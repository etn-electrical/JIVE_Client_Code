/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "uart_events.h"
 #include <wifi_provisioning/manager.h>// <wifi_provisioning/manager.h>//


#include "Prod_Spec_LTK_ESP32.h"
#include "Prod_Spec.h"
#include <wifi_provisioning/manager.h>
#include "Prod_Spec_Wifi.h"


#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sys.h"


#include "esp_efuse.h"
#include "esp_efuse_table.h"
//#include "sdkconfig.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp32/rom/crc.h"
#include "esp_crc.h"
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include "Breaker.h"
#include "flash.h"
#include "EventManager.h"
#include "ErrorLog.h"
#include "sblcp.h"

extern bool ShouldForceDps;
extern bool ShouldConnectToIot;
/* -------------------------------------------------------------------------
 * The following details the custom parameters to be used by the 
 * test functions such as Test_AddingConnectionString(), 
 * Test_AddingWifiSSID(), ect.
 * Ensure that only one definition is commented in for each string at a time
 * ------------------------------------------------------------------------- */

static char const TEST_ConnectionString[] = //Eaton - Default1
											//"HostName=EatonProdCRDSWDeviceIothub1.azure-devices.net;DeviceId=cce3d89a-1601-4f33-a7ba-469282c9a13b;SharedAccessKey=XmXQ3rryxshOcaXVBZZRhIQl/uIZQ8MnO1oGHRdE+Po=";
											//Eaton - Default2
											//"HostName=EatonCRDSWDeviceIothub1.azure-devices.net;DeviceId=a1da591c-db77-4808-8fe7-67d233276477;SharedAccessKey=wWSelSch25kEOcMk0Llr187qCotmjTYLdJ3wkRLu324=";
		                                    //"HostName=EatonCRDSWDevice4Iothub1.azure-devices.net;DeviceId=67f3b1c9-d498-4460-bcf8-811ab220be57;SharedAccessKey=/9L2MTQC+gYZC6WX3KreqJ6J/GG8LepgyBw6xAjXSnQ=";
											//"HostName=EatonCRDSWDevice4Iothub1.azure-devices.net;DeviceId=c808196c-f40f-4721-b68c-262ac875aab4;SharedAccessKey=mDvLHR+UNU1HeeFq8vleRpwovJX7qkseDi8iEk6CNN8=";
											"";

static char const TEST_WifiSSID[] = 		//Eaton - Default1
											"";

static char const TEST_WifiPW[] = 			//Eaton - Default1
											"";

static char const TEST_BuildType[] =		//Eaton - Default1
											"AhmedBuild";
											
static char const TEST_PartNumber[] = 		//Eaton - Default1
											"AhmedDimmer";

static char const TEST_DpsEndpoint[] =      "global.azure-devices-provisioning.net";

static char const TEST_DpsIdScope[] =       "0ne009B3C25";

static char const TEST_DpsDeviceRegId[] =	"3d5e7a78-62c2-479f-97ea-20dfc4d50b2d";

static char const TEST_DpsSymmetricKey[] =  "TeuuJRjDYKs4txolrmI82WBjpFIoa9sy7dFlMhJFM94=";

static char const TEST_DpsConcatenatedMessage[] = 
											//"DpsDevRegID:bea77eeb-0850-4f94-91f0-1384b1054fb7;DpsEndpoint:global.azure-devices-provisioning.net;DpsIdScope:0ne009B3C25;DpsSymmetricKey:4JXeChuE6pjDwvMKxhago6gPW7kekWD9XBcFM1w2CAQ=";
											"DpsDevRegID:3d5e7a78-62c2-479f-97ea-20dfc4d50b2d;DpsEndpoint:global.azure-devices-provisioning.net;DpsIdScope:0ne009B3C25;DpsSymmetricKey:TeuuJRjDYKs4txolrmI82WBjpFIoa9sy7dFlMhJFM94=";
void send_crc_error_response();

extern void StartProvisioning(void);
//extern bool g_handleStatusUpdate;
extern sblcpStatusInfo_st replyStatusInfo;
static const int RX_BUF_SIZE = 1024;

static int ConnectionStringLengthReceived = 0;
static const char *TAG = "COM1";
//#define TXD_PIN (GPIO_NUM_4)
//#define RXD_PIN (GPIO_NUM_5)
static const char *RX_TASK_TAG = "RX_BYTES";
static const char *TX_TASK_TAG = "TX_BYTES";
static bool BuildTypeWasProgrammed = false;

bool guart_recvd_err = false;
uint8_t buf[4] = { 0xFA, 0x11, 0x28, 0x33 };

M2M_UART_COMMN gm2m_uart_comm = {0};

uint8_t msg_counter;
float st_temperature;

xQueueHandle g_send_uart_event_queue = NULL;
TaskHandle_t g_uart_send_task_handle;

extern DRAM_ATTR Device_Info_str DeviceInfo;
extern uint8_t gStm_Firmware_Version[];//major.minor.patch versions
extern uint32_t gf_data;
extern uint32_t hall_data_adc;

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
  //  uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_0, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

/**
* @brief send packet to M2M UART
*
* @param const char*  - logger name
* @param const char*  - data bytes to be sent on UART
* @return txBytes     - total sent bytes on UART
*/
int send_packet_to_m2m_uart(const char* logName, const char* data, const int len)
{
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);

#if UART_DEBUG_MESSAGE
    ESP_LOGI(TAG, "\nWrote %d bytes\n", txBytes);
    if(len)
    {
    	 const void *buffer = data;
        for (int i=0; i<len; ++i) {
            printf("%02x ", ((uint8_t*)buffer)[i]);
        }
        ESP_LOGI(TAG, "\len:[%d] bytes\n", len);
    }
#endif

    ESP_LOG_BUFFER_HEXDUMP(TX_TASK_TAG, data, sizeof(M2M_UART_COMMN), ESP_LOG_INFO);
    return txBytes;
}

static void rx_task(void *arg)
{
	uint16_t  crc_cal_in = 0;
	uint16_t  crc_cal_out = 0;
	uint16_t  tx_data_len = 0;
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    uint8_t* tx_data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        memset (data,0,RX_BUF_SIZE);
        memset (tx_data,0,RX_BUF_SIZE);
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE-1, 1000 / portTICK_RATE_MS);
        if (rxBytes > 3) {
            static const char *TX_TASK_TAG = "TX_TASK1";
                if(data[0] == UART_DEVICE_TYPE)
                {
					if(data[1] == UART_CONNECT_MESSAGE)  //Connect
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							tx_data[0]= UART_DEVICE_TYPE;
							tx_data[1]= UART_CONNECT_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=0x01;
							tx_data[4]=0xaa;
						//	tx_data[4]=DeviceInfo.Installed;
							tx_data_len = 5;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[5]=(uint8_t)(crc_cal_out>>8);
							tx_data[6]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = 7;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
                            DeviceInfo.DeviceInFactoryMode = DEVICE_IS_IN_FACTORY_MODE;
                            BuildTypeWasProgrammed = false;
							ShouldConnectToIot = false;
					//		DeviceInfo.FLAG_WIFI_DISABLE = 1;
						}
						else
						{
							send_crc_error_response();
						}
                        
					}
					else if (data[1] == UART_SEND_CONNECTION_STRING_MESSAGE)  //Send the Connection-String
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						//uint8_t *inbuf;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
                                                     ConnectionStringLengthReceived = inlen = data[3];
							uint8_t *inbuf = &data[4];
					        memset ((char *)&DeviceInfo.ConnectionString,0, sizeof (DeviceInfo.ConnectionString));
					        memcpy ((char *)&DeviceInfo.ConnectionString, inbuf, inlen);
					        SetConnectStringInfo();
					        DeviceInfo.ProvisionState=Provisioned;//store the connect string
							tx_data[0]= UART_DEVICE_TYPE;
							tx_data[1]=UART_SEND_CONNECTION_STRING_REPLY_MESSAGE;
							tx_data[2]= 0x00;
							tx_data[3]=inlen;
							uint8_t i;
							for(i=0;i<inlen;i++)
							{
                                tx_data[i+4] = data[i+4];
 
							}
							//tx_data[4]=0xaa;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen+4]=(uint8_t)(crc_cal_out>>8);
							tx_data[inlen+5]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = 163;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
							//WriteStringToFlash(EEOFFSET_CONNECTION_STRING_LABEL,DeviceInfo.ConnectionString,sizeof(DeviceInfo.ConnectionString));
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_CONNECTION_STRING_MESSAGE)  //get the Connection-String
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
                                                        GetConnectStringInfo();
							inlen = strnlen(DeviceInfo.ConnectionString, ConnectionStringLength);
							uint8_t *inbuf = &tx_data[4];
							//SetConnectStringInfo();
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_CONNECTION_STRING_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]= inlen;
							memcpy (inbuf,(char *)&DeviceInfo.ConnectionString, inlen);
							//tx_data[4]=0xaa;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen+4]=(uint8_t)(crc_cal_out>>8);
							tx_data[inlen+5]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_FIRMWARE_VERSION_MESSAGE)  //Get the firmware version
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							//ESP_LOGI(TAG, "deal_data2");
							inlen = 4;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_FIRMWARE_VERSION_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							uint64_t ver = MY_FIRMWARE_VERSION;
							memcpy (inbuf,(char *)&ver, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen+4]=(uint8_t)(crc_cal_out>>8);
							tx_data[inlen+5]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_DEVICE_ID_MESSAGE)  //Get the Device ID
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 36;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_DEVICE_ID_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=0x24;
                            GetConnectStringInfo();
							memcpy (inbuf,(char *)&DeviceInfo.DeviceId, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen+4]=(uint8_t)(crc_cal_out>>8);
							tx_data[inlen+5]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_SHARED_ACCESS_KEY_MESSAGE)  //Get the Shared-Access-Key
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 44;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_SHARED_ACCESS_KEY_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=0x24;
                            GetConnectStringInfo();
							memcpy (inbuf,(char *)&DeviceInfo.SharedAccessKey, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[40]=(uint8_t)(crc_cal_out>>8);
							tx_data[41]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_DEVICE_PROFILE_TYPE_MESSAGE)  //Get the Device Profile ID
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							#ifdef _OLD_APP_NO_BREAKER_
								char ProfileID[37] = "7883554d-89df-43c4-80ad-65be692c9296";
							#else
							         char ProfileID[37] = "58af4fd1-b1f8-ec11-b47a-0050f2f4e861";//"7883554d-89df-43c4-80ad-65be692c9296";
							#endif
							inlen = strlen(ProfileID);
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_DEVICE_PROFILE_TYPE_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							memcpy (inbuf,(char *)&ProfileID, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[40]=(uint8_t)(crc_cal_out>>8);
							tx_data[41]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_LOCK_DEVICE_MESSAGE)  //Lock the device
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 0;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_LOCK_DEVICE_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=0x00;
						//	memcpy (inbuf,(char *)&DeviceInfo.SharedAccessKey, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
				//			periph_module_disable(PERIPH_UART0_MODULE);   //disable Uart0
							//Ahmed For the dimmer it shouldn't be the case DeviceInfo.Installed = DEVICE_PROVISIONED;
							//Ahmed For the dimmer it shouldn't be the case WriteByteToFlash(EEOFFSET_PROVISION_LABEL , DeviceInfo.Installed);

#if 0
                            if (BuildTypeWasProgrammed == true)
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
                                  //  ESP_LOGI(TAG, "Done");
                                    DeviceInfo.TheDeviceIsLocked = true;
                                    WriteByteToFlash(EEOFFSET_DEVICE_LOCKED_LABEL,DEVICE_IS_LOCKED );                            
                                }
                            }
#endif
                            ResetDevice(DEVICE_RESET, true);

						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_WIFI_STATUS_MESSAGE)  //Get the WiFi status
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 1;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_WIFI_STATUS_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							tx_data[4]=DeviceInfo.WiFiConnected;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
							//periph_module_disable(PERIPH_UART0_MODULE);   //disable Uart0
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_SET_SSID_MESSAGE)  //Set SSID
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							//inlen = 1;
							inlen = data[3];
							uint8_t *inbuf = &data[4];
							memset ((char *)&DeviceInfo.Ssid,0, sizeof (DeviceInfo.Ssid));
							//memset ((const char *) wifi_sta_cfg->ssid,0, sizeof (DeviceInfo.Ssid));
					        memcpy ((char *)&DeviceInfo.Ssid, inbuf, inlen);
							tx_data[0]= UART_DEVICE_TYPE;
							tx_data[1]=UART_SET_SSID_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							uint8_t i;
							for(i=0;i<inlen;i++)
							{
								tx_data[i+4] = DeviceInfo.Ssid[i];
							}
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
					        DCI_Update_SSID();
                            GetDeviceSsid();                            

						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_SSID_MESSAGE)  //Get SSID
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
                            GetDeviceSsid();
							inlen = strlen(DeviceInfo.Ssid);
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_SSID_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							memcpy (inbuf,(char *)&DeviceInfo.Ssid, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);

						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_SET_WIFI_PASSWORD_MESSAGE)  //Set password
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							//inlen = 1;
							inlen = data[3];
							uint8_t *inbuf = &data[4];
							memset ((char *)&DeviceInfo.PassWord,0, sizeof (DeviceInfo.PassWord));
					        memcpy ((char *)&DeviceInfo.PassWord, inbuf, inlen);
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_SET_WIFI_PASSWORD_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							uint8_t i;
							for(i=0;i<inlen;i++)
							{
								tx_data[i+4] = DeviceInfo.PassWord[i];
							}
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
					        DCI_Update_DevicePassWord();
                            GetDevicePassWord();                            

						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_WIFI_PASSWORD_MESSAGE)  //Get password
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
                            GetDevicePassWord();
							inlen = strlen(DeviceInfo.PassWord);
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_WIFI_PASSWORD_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							memcpy (inbuf,(char *)&DeviceInfo.PassWord, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);

						}
						else
						{
							send_crc_error_response();
						}
					}

					else if (data[1] == UART_GET_MAC_ADDRESS_MESSAGE)  //Get MAC address
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = sizeof(DeviceInfo.MacAddress);
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_MAC_ADDRESS_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							memcpy (inbuf,(char *)&DeviceInfo.MacAddress, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);

						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_CONNECT_WIFI_MESSAGE)  //Connect to WiFi
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 1;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_CONNECT_WIFI_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							//Ahmed memcpy (inbuf,(char *)&DeviceInfo.Device_Connection_status, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
                            Factory_WiFi_Connect();
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_DISCONNECT_WIFI_MESSAGE)  //Disconnect  WiFi
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 0;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_DISCONNECT_WIFI_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);

                            ResetDevice(PROVISIONING_RESET, false);

						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_CHECK_IOT_STATUS_MESSAGE)  //check IOT status
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 1;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_CHECK_IOT_STATUS_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							tx_data[4]=DeviceInfo.IoTConnected;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);

						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_ERASE_WIFI_CREDENTIAL_MESSAGE)  //Erase WiFi credential
					{
                        // Will do nothing other than replying since it is redundant
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 0;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_ERASE_WIFI_CREDENTIAL_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;

							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
                            ResetDevice(PROVISIONING_RESET, false);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_CONNECT_IOT_MESSAGE)  //Connect to IOT
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							ShouldConnectToIot = true;
							ShouldForceDps = false;
							inlen = 0;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_CONNECT_IOT_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
							//INIT_WIFi_iot();
							//DeviceInfo.ProvisionState=Provisioned;


							//Ahmed DeviceInfo.Installed = COMMISIONED;
							//Ahmed DeviceInfo.Connected = true;

						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_RESET_DEVICE_MESSAGE)  //Reset the device
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 0;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_RESET_DEVICE_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
							esp_restart();
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_SET_BUILD_TYPE_MESSAGE)  //receiving the build type
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						//uint8_t *inbuf;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = data[3];
							uint8_t *inbuf = &data[4];
					        memset ((char *)&DeviceInfo.BuildType,0, sizeof (DeviceInfo.BuildType));
					        memcpy ((char *)&DeviceInfo.BuildType, inbuf, inlen);
                            BuildTypeWasProgrammed = true;
					        WriteDataToFlash(EEOFFSET_BUILD_TYPE_LABEL,DeviceInfo.BuildType,sizeof(DeviceInfo.BuildType));
                            WriteByteToFlash(EEOFFSET_BUILD_TYPE_SAVED_LABEL,BUILD_TYPE_SAVED );
							tx_data[0]= UART_DEVICE_TYPE;
							tx_data[1]=UART_SET_BUILD_TYPE_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							uint8_t i;
							for(i=0;i<inlen;i++)
							{
								tx_data[i+4] = data[i+4];
							}
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen+4]=(uint8_t)(crc_cal_out>>8);
							tx_data[inlen+5]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_BUILD_TYPE_MESSAGE)  //Get The build type
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = strlen(DeviceInfo.BuildType);
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_BUILD_TYPE_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							memcpy (inbuf,(char *)&DeviceInfo.BuildType, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);

						}
						else
						{
							send_crc_error_response();
						}
					}

					else if (data[1] == UART_SET_PROGRAMMED_PART_NUMBER_MESSAGE)  //receiving PartNumber
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						//uint8_t *inbuf;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = data[3];
							uint8_t *inbuf = &data[4];
					        memset ((char *)&DeviceInfo.NewProgramedPartNumber,0, sizeof (DeviceInfo.NewProgramedPartNumber));
					        memcpy ((char *)&DeviceInfo.NewProgramedPartNumber, inbuf, inlen);
					        WriteDataToFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_LABEL,DeviceInfo.NewProgramedPartNumber,sizeof(DeviceInfo.NewProgramedPartNumber));
                            WriteByteToFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_SAVED_LABEL,NEW_PROGRAMMED_PART_NO_SAVED );
							tx_data[0]= UART_DEVICE_TYPE;
							tx_data[1]=UART_SET_PROGRAMMED_PART_NUMBER_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							uint8_t i;
							for(i=0;i<inlen;i++)
							{
								tx_data[i+4] = data[i+4];
							}
							//tx_data[4]=0xaa;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen+4]=(uint8_t)(crc_cal_out>>8);
							tx_data[inlen+5]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_PROGRAMMED_PART_NUMBER_MESSAGE)  //Get new Part number
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3];
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = strlen(DeviceInfo.NewProgramedPartNumber);
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_PROGRAMMED_PART_NUMBER_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							memcpy (inbuf,(char *)&DeviceInfo.NewProgramedPartNumber, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);

						}
						else
						{
							send_crc_error_response();
						}
					}                    
					else if (data[1] == UART_SET_IOT_DPS_MESSAGE) //Set the DPS (concatenated) fields
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
                            // Max data length is <255, so OK to ignore hi length byte in msg (data[2]) /response (tx_data[2])
                            inlen = data[3];
							uint8_t *inbuf = &data[4];
							memset(DeviceInfo.DpsConcatenatedMessage, 0, sizeof(DeviceInfo.DpsConcatenatedMessage));
							memcpy(DeviceInfo.DpsConcatenatedMessage, inbuf, inlen);
							SetDPSConcatenatedMessageToIndividualFields();

							tx_data[0]= UART_DEVICE_TYPE;
							tx_data[1]=UART_SET_IOT_DPS_REPLY_MESSAGE;
							tx_data[2]= 0x00;
							tx_data[3]=inlen;
							uint8_t i;
							for(i=0;i<inlen;i++)
							{
                                tx_data[i+4] = data[i+4]; // similar to set iot connectionstring -- mirror msg data as response data
 
							}
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen+4]=(uint8_t)(crc_cal_out>>8);
							tx_data[inlen+5]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_GET_IOT_DPS_MESSAGE) //Get the DPS (concatenated) fields
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen; // no data sent for this message type
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							// Max field length is <255, so OK to zero out hi length byte in response (tx_data[2])
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_IOT_DPS_REPLY_MESSAGE;
							tx_data[2]=0x00;
							GetDPSConcatenatedMessageFromIndividualFields();
							inlen = strlen(DeviceInfo.DpsConcatenatedMessage);
							tx_data[3]= inlen;
							uint8_t *inbuf = &tx_data[4];
							memcpy(inbuf, DeviceInfo.DpsConcatenatedMessage, inlen);
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen+4]=(uint8_t)(crc_cal_out>>8);
							tx_data[inlen+5]=(uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_CONNECT_DPS_MESSAGE)  //Connect to DPS and get IOT connection info/string
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen; // no data sent for this message type
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							ShouldForceDps = true;
							inlen = 0; // no data to be returned
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_CONNECT_DPS_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
							//Expected flow is to connect to WiFi and then DPS will be contacted if needed
						}
						else
						{
							send_crc_error_response();
						}
					}
					else if (data[1] == UART_VERIFY_DPS_CONNECTION_MESSAGE)  // check that DPS registration succeeded and returned IOT connection info
					{
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						ssize_t inlen;
						//inlen = data[3]; // shoudl be 0 for this message
						crc_cal_in = esp_crc16_le(0, data, (rxBytes-2));
						temp_crc_cal_in_H = (uint8_t)(crc_cal_in>>8);
						temp_crc_cal_in_L = (uint8_t)(crc_cal_in&0x00ff);
						if((temp_crc_cal_in_H == data[rxBytes-2]) && (temp_crc_cal_in_L == data[rxBytes-1]))
						{
							inlen = 1;
							uint8_t *inbuf = &tx_data[4];
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_VERIFY_DPS_CONNECTION_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]=inlen;
							tx_data[4]=DeviceInfo.DpsReturnedConnectionStringSaved?0x01:0x00;
							tx_data_len = inlen + 4;
							crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
							tx_data[inlen + 4] = (uint8_t)(crc_cal_out>>8);
							tx_data[inlen + 5] = (uint8_t)(crc_cal_out&0x00ff);
							tx_data_len = tx_data_len + 2;
							uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						}
						else
						{
							send_crc_error_response();
						}
					}
					else
					{
						uint16_t  crc_cal_out = 0;
						uint16_t  tx_data_len = 0;
						uint8_t* tx_data = (uint8_t*) malloc(RX_BUF_SIZE+1);
						uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
						tx_data[0]=UART_DEVICE_TYPE;
						tx_data[1]=UART_COMMAND_ERROR_REPLY_MESSAGE;
						tx_data[2]=0x00;
						tx_data[3]=0x00;
						tx_data_len = 4;
						crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
						tx_data[4]=(uint8_t)(crc_cal_out>>8);
						tx_data[5]=(uint8_t)(crc_cal_out&0x00ff);
						tx_data_len = 6;
						uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
						free(tx_data);
					}
                }

                vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    free(data);
    free(tx_data);
}

void send_crc_error_response()
{
	uint16_t  crc_cal_out = 0;
	uint16_t  tx_data_len = 0;
	uint8_t* tx_data = (uint8_t*) malloc(RX_BUF_SIZE+1);
	uint8_t temp_crc_cal_in_H, temp_crc_cal_in_L;
	tx_data[0]=UART_DEVICE_TYPE;
	tx_data[1]=UART_CRC_ERROR_REPLY_MESSAGE;
	tx_data[2]=0x00;
	tx_data[3]=0x00;
	tx_data_len = 4;
	crc_cal_out = esp_crc16_le(0, tx_data, tx_data_len);
	tx_data[4]=(uint8_t)(crc_cal_out>>8);
	tx_data[5]=(uint8_t)(crc_cal_out&0x00ff);
	tx_data_len = 6;
	uart_write_bytes(UART_NUM_0,(const char *) tx_data, tx_data_len);
	free(tx_data);
}

/**
* @brief Calculate CRC8 value
*
* @param data: Data buffer that used to calculate the CRC value
* @param length: Length of the data buffer
* @return CRC8 value
*/
uint8_t CRC8(const uint8_t *data,int length)
{
	//printf("data::%x length:%x\n",*data,length);
	uint8_t crc = 0x00;
	uint8_t extract;
	uint8_t sum;
	for(int i=0;i<length;i++)
	{
		extract = *data;
		for (uint8_t tempI = 8; tempI; tempI--)
		{
			sum = (crc ^ extract) & 0x01;
			crc >>= 1;
			if (sum)
			{
				crc ^= 0x8C;
			}
			extract >>= 1;
		}
		data++;
	}
	//printf("crc::%x\n",crc);
	return crc;
}


/**
* @brief Prepare M2M protocol UART command to send to protection MCU
*
* @param M2M_UART_COMMN : pointer to the M2M protocol command format variable
* @param UART_COMMANDS  : M2M UART Command Type value
* @param payload        : M2M UART Command payload value
* @return success - 0, failure - (-1)
*/
int prepare_uart_command(M2M_UART_COMMN *pm2m_uart_comm, UART_COMMANDS uart_cmd, uint8_t payload_1, uint8_t payload_2, uint8_t payload_3)
{
	msg_counter++;
	//uint8_t packet_data = (START_BYTE + uart_cmd + sizeof(M2M_UART_COMMN) + payload_1 + payload_2 + payload_3 + END_BYTE);
	pm2m_uart_comm->start_flag      = START_BYTE;
	pm2m_uart_comm->cmd             = uart_cmd;
	pm2m_uart_comm->msg_num			= msg_counter;
	pm2m_uart_comm->len             = sizeof(M2M_UART_COMMN);
	pm2m_uart_comm->payload_1       = payload_1;
	pm2m_uart_comm->payload_2       = payload_2;
	pm2m_uart_comm->payload_3       = payload_3;
	pm2m_uart_comm->crc             = CRC8((const uint8_t *)pm2m_uart_comm, (pm2m_uart_comm->len-2));
	pm2m_uart_comm->end_flag        = END_BYTE;

	return 0;
}

/**
* @brief format and send an ack message for UART commands.
*
* @param M2M_UART_COMMN : pointer to the M2M protocol command format variable
* @param UART_COMMANDS  : M2M UART Command Type value
* @param payload        : M2M UART Command payload value
* @return void
*/
void send_uart_ack(M2M_UART_COMMN *pm2m_uart_comm)
{
	static M2M_UART_COMMN m2m_uart_comm;

	m2m_uart_comm.start_flag      = START_BYTE;
	m2m_uart_comm.cmd             = PROTECTION_MCU_ACK;
	m2m_uart_comm.msg_num			= pm2m_uart_comm->msg_num;
	m2m_uart_comm.len             = sizeof(M2M_UART_COMMN);
	m2m_uart_comm.payload_1       = POSITIVE_ACK;
	m2m_uart_comm.payload_2       = NO_PAYLOAD;
	m2m_uart_comm.payload_3       = NO_PAYLOAD;
	m2m_uart_comm.crc             = CRC8((const uint8_t *)&m2m_uart_comm, (m2m_uart_comm.len-2));
	m2m_uart_comm.end_flag        = END_BYTE;

	send_packet_to_m2m_uart(TX_TASK_TAG, (const char*)&m2m_uart_comm, sizeof(M2M_UART_COMMN));

}

/**
* @brief Process breaker fault status M2M UART CMD from protection MCU
*
* @param M2M_UART_COMMN : pointer to the M2M protocol command format variable
* @return success - 0, failure - (-1)
*/
int process_breaker_fault_state(M2M_UART_COMMN *pm2m_uart_comm)
{
	//ets_printf("Process Breaker Fault State::[%d]\n",pm2m_uart_comm->payload_1);
	//Payload2 to be used for the primary trip change state at ESP32-Communication side in future.
	//log trip/error code to nvm
	ErrLog(pm2m_uart_comm->payload_1);

	return 0;
}

/**
* @brief Process breaker open/close status M2M UART CMD from protection MCU
*
* @param M2M_UART_COMMN : pointer to the M2M protocol command format variable
* @return success - 0, failure - (-1)
*/
int process_open_close(M2M_UART_COMMN *pm2m_uart_comm)
{
//	ets_printf("Received Breaker ON/OFF State::[%d]\n",pm2m_uart_comm->payload_1);
//	ets_printf("DeviceInfo.SecondaryContactState::[%d]\n",DeviceInfo.SecondaryContactState);


//	if((pm2m_uart_comm->payload_1 >= SS_OPEN) && (pm2m_uart_comm->payload_1 <= SS_UNKNOWN))
//	{
//		DeviceInfo.SecondaryContactState = pm2m_uart_comm->payload_1;
//	}
	//this flag is set to indicate SBLCP that secondary status is updated.
//	g_handleStatusUpdate = 1;
	return 0;
}
void update_temperature(M2M_UART_COMMN *pm2m_uart_comm_temp)
{
	// It is possible the temperature reading is negative
	int16_t temp_int = (int16_t)(pm2m_uart_comm_temp->payload_1<< 8 | pm2m_uart_comm_temp->payload_2);
	st_temperature= (float) temp_int;
//	printf("Temperature converted is: [%.1f]\n",st_temperature);

}

void process_breaker_status(M2M_UART_COMMN *pm2m_uart_comm)
{
	//printf("process_breaker_status\n");
	static uint8_t prev_secondary_state = SS_OPEN;
	DeviceInfo.PrimaryContactState = pm2m_uart_comm->payload_1;
	DeviceInfo.SecondaryContactState = pm2m_uart_comm->payload_2;
	DeviceInfo.PathStatus = pm2m_uart_comm->payload_3;

	if(prev_secondary_state != DeviceInfo.SecondaryContactState)
	{
		prev_secondary_state = (uint8_t)DeviceInfo.SecondaryContactState;
		EERAM_Set_Last_State(prev_secondary_state);
		if((DeviceInfo.SecondaryContactState == SS_OPEN) || (DeviceInfo.SecondaryContactState == SS_CLOSED))
		{
			//check if if status change is due to SBLCP command or not
			//if its due to SET command from SBLCP, then only update handle status change to SBLCP;
			if(get_SblcpCommandStatus() == true)
			{
				sblcp_updateReplyMsgStatus(true);
			}
		}
	}
	//ets_printf("DeviceInfo states::[%d],[%d],[%d]\n",DeviceInfo.PrimaryContactState,DeviceInfo.SecondaryContactState,DeviceInfo.PathStatus);
	//ets_printf("DeviceInfo states::[%d],[%d],[%d]\n",DeviceInfo.PrimaryContactState,DeviceInfo.SecondaryContactState,DeviceInfo.PathStatus);
}


/**
* @brief Process Button press 1
*
* @param M2M_UART_COMMN : pointer to the M2M protocol command format variable
* @return success - 0, failure - (-1)
*/
int process_button_press_1(M2M_UART_COMMN *pm2m_uart_comm)
{
	printf("process_button_press_1\n");
	return 0;
}

void process_gf_raw_stats(M2M_UART_COMMN *pm2m_uart_comm)
{
	gf_data = (uint32_t)pm2m_uart_comm->payload_3 | (uint32_t)(pm2m_uart_comm->payload_2 << 8) | (uint32_t)(pm2m_uart_comm->payload_1 << 16);
	//ets_printf("gf_data::[%d]\n",gf_data);
}

void process_HAL_raw_stats(M2M_UART_COMMN *pm2m_uart_comm)
{
	hall_data_adc = (uint32_t)pm2m_uart_comm->payload_3 | (uint32_t)(pm2m_uart_comm->payload_2 << 8) | (uint32_t)(pm2m_uart_comm->payload_1 << 16);
	//ets_printf("hall_data_adc::[%d]\n",hall_data_adc);
}
/**
* @brief Process switch 2 press for breaker provision
*
* @param M2M_UART_COMMN : pointer to the M2M protocol command format variable
* @return success - 0, failure - (-1)
*/
int process_switch2_press_provision(M2M_UART_COMMN *pm2m_uart_comm)
{
	StartProvisioning();
	return 0;
}

/**
* @brief Preserve Protection FW Version at Communication side
*
* @param M2M_UART_COMMN : pointer to the M2M protocol command format variable
* @return void
*/
void process_protection_fw_version(M2M_UART_COMMN *pm2m_uart_comm)
{
	static uint8_t count = 0;
	if(pm2m_uart_comm != NULL)
	{
		gStm_Firmware_Version[count++] = pm2m_uart_comm->payload_1;
		gStm_Firmware_Version[count++] = pm2m_uart_comm->payload_2;
		gStm_Firmware_Version[count++] = pm2m_uart_comm->payload_3;
	}
	if(count >= 7)
	{
		count = 0;
		DeviceInfo.Protection_FW_Version_Read = true;

		strncpy(DeviceInfo.STM_firmware_ver, (char *)gStm_Firmware_Version, sizeof(DeviceInfo.STM_firmware_ver) - 1);
	}

}

/**
* @brief Process M2M protocol UART command from protection MCU
*
* @param M2M_UART_COMMN : pointer to the M2M protocol command format variable
* @return success - 0, failure - (-1)
*/
int process_uart_packet(M2M_UART_COMMN *pm2m_uart_comm)
{
	switch(pm2m_uart_comm->cmd)
	{
		case BREAKER_FAULT_STATE:
			send_uart_ack(pm2m_uart_comm);
			process_breaker_fault_state(pm2m_uart_comm);
			break;

		case BREAKER_OPEN_CLOSE:
			send_uart_ack(pm2m_uart_comm);
			process_open_close(pm2m_uart_comm);
			break;

		case COMM_MCU_ACK:
			break;

		case PROTECTION_MCU_ACK:
			break;

		case ESP_FACTORY_RESET:
			send_uart_ack(pm2m_uart_comm);
			ResetDevice(MANUFACTURING_RESET, true);
			break;

		case READ_ESP_UART_STATUS:
		{
			if(DeviceInfo.M2MUartStatus == ESP_UART_READY)
			{
//				ets_printf("\n############## ESP32 UART READY : M2M UP ################\n");
				prepare_uart_command(pm2m_uart_comm, READ_ESP_UART_STATUS, ESP_UART_READY, NO_PAYLOAD_2, NO_PAYLOAD_2);
			}
			else
			{
				prepare_uart_command(pm2m_uart_comm, READ_ESP_UART_STATUS, NO_PAYLOAD, NO_PAYLOAD_2, NO_PAYLOAD_2);
			}
			send_packet_to_m2m_uart(TX_TASK_TAG, (const char*)pm2m_uart_comm, sizeof(M2M_UART_COMMN));
		}
		break;

		case BUTTON_PRESS_1_STATUS:
			send_uart_ack(pm2m_uart_comm);
			process_button_press_1(pm2m_uart_comm);
		break;

		case BREAKER_STATUS:
			process_breaker_status(pm2m_uart_comm);
		break;

		case BREAKER_GF_RAW:
			process_gf_raw_stats(pm2m_uart_comm);
		break;

		case BREAKER_HAL_RAW:
			process_HAL_raw_stats(pm2m_uart_comm);
		break;

		case UPDATE_TEMPERATURE:
			send_uart_ack(pm2m_uart_comm);
			update_temperature(pm2m_uart_comm);
			break;

		case SWITCH2_PRESS_PROVISION:
			send_uart_ack(pm2m_uart_comm);
			process_switch2_press_provision(pm2m_uart_comm);
		break;

		case READ_COMMUNICATION_MCU_CMD:
		{
			//if STM-ESP protection status is established, then read STM FW Version
			if((DeviceInfo.M2MUartStatus == ESP_UART_READY)
					&& (!DeviceInfo.Protection_FW_Version_Read))
			{
				prepare_uart_command(pm2m_uart_comm, BREAKER_PRTOTECTION_FW_VERSION, NO_PAYLOAD, NO_PAYLOAD_2, NO_PAYLOAD_2);
				send_packet_to_m2m_uart(TX_TASK_TAG, (const char*)pm2m_uart_comm, sizeof(M2M_UART_COMMN));
			}
		}
		break;

		case READ_WIFI_STATUS:
			break;

		case READ_ESP_STATUS:
			break;

		case ERROR_RESPONSE:
			break;

		case BREAKER_PRTOTECTION_FW_VERSION:
		{
			process_protection_fw_version(pm2m_uart_comm);
		}
		break;
		default:
		{
			//Error response to user/cloud or if LED is finalized then use LED to show error
			//if error response received update a flag or use a queue to always send a command to
			//protection MCU
			guart_recvd_err =  true;
			//prepare uart command for crc error response
			prepare_uart_command(pm2m_uart_comm, ERROR_RESPONSE, INVALID_CMD_ERROR_RESPONSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
			UART_Send_Event_To_Queue((void *)pm2m_uart_comm);
		}
		break;
	}
	return 0;
}

/**
* @brief M2M UART Protocol Receiver Task
*
* @param arg * - arguments to be passed to task
* @return void - task specific to file
*/
static void m2m_uart_rx_task(void *arg)
{
    static M2M_UART_COMMN m2m_uart_comm;
    static uint8_t last_msg_num = 0xFF;
    static uint8_t last_msg_crc = 0;
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(2 * sizeof(M2M_UART_COMMN));
    uint8_t recv_byte = 0;
    uint8_t crc8 = 0;
    uint8_t packet_data = 0;//need to remove
    int rxBytes = 0;

    while (1)
    {
    	crc8 = packet_data = rxBytes = 0;
    	memset(data, 0x00, (2 * sizeof(M2M_UART_COMMN)));
    	rxBytes = uart_read_bytes(UART_NUM_1, data, sizeof(M2M_UART_COMMN), 1000 / portTICK_RATE_MS);
        if (rxBytes > 0)
        {
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes", rxBytes);
            memcpy((void *)&m2m_uart_comm, (void *)data, sizeof(M2M_UART_COMMN));
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, sizeof(M2M_UART_COMMN), ESP_LOG_INFO);
            //validate the M2M packet with start and end byte
            if((m2m_uart_comm.start_flag == START_BYTE) && (m2m_uart_comm.end_flag == END_BYTE))
            {
            	//need to add  payload 2 when led and blinking rate code support is added
                packet_data = (m2m_uart_comm.start_flag + m2m_uart_comm.cmd + m2m_uart_comm.msg_num + m2m_uart_comm.len + m2m_uart_comm.payload_1 + m2m_uart_comm.end_flag);
                //subtract 2 to ignore the crc and end flag.
                crc8 = CRC8((const uint8_t *)&m2m_uart_comm, (m2m_uart_comm.len-2));
                //printf("crc8 [%x]\n", crc8);
                //crc check
                if(crc8 != m2m_uart_comm.crc)
                {
                	    //read error command will send error response
          				prepare_uart_command(&m2m_uart_comm, ERROR_RESPONSE, CRC_ERROR_RESPONSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
    	      			//send uart command
    				    send_packet_to_m2m_uart(TX_TASK_TAG, (const char*)&m2m_uart_comm, sizeof(M2M_UART_COMMN));
                }
                else
                {
					        //process uart cmd
                		if((last_msg_num != m2m_uart_comm.msg_num) || (m2m_uart_comm.crc != last_msg_crc))
                		{
					        process_uart_packet(&m2m_uart_comm);
					        //Should move to below code to ensure proper handling however function not setup correcty for it at this time.
					        //if(process_uart_packet(&m2m_uart_comm) == 0)//no error
					        //{
					        //	last_msg_num = m2m_uart_comm.msg_num;
							//	last_msg_crc = m2m_uart_comm.crc;
					        //}
                		}
                		else
                		{
                			send_uart_ack(&m2m_uart_comm);
                		}
                		last_msg_num = m2m_uart_comm.msg_num;
                		last_msg_crc = m2m_uart_comm.crc;
                }
            }
            else
            {
        			//prepare uart command for crc error response
        			prepare_uart_command(&m2m_uart_comm, ERROR_RESPONSE, INVALID_CMD_ERROR_RESPONSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
        			send_packet_to_m2m_uart(TX_TASK_TAG, (const char*)&m2m_uart_comm, sizeof(M2M_UART_COMMN));
            }
        }
        uart_flush(UART_NUM_1);
    }
    free(data);
}



/**
 * @brief      UART send event task to send M2M commands to STM32
 * @param[in]  None
 * @retval     void
*/
void Uart_Event_Send_Task(void* arg)
{
	while(1)
	{
		if (xQueueReceive(g_send_uart_event_queue, &gm2m_uart_comm, portMAX_DELAY)) // Wait for an event
		{
			//printf("\n################### gm2m_uart_comm.cmd [%d] ##########################\n",gm2m_uart_comm.cmd);
			//M2M command update to STM
			if(gm2m_uart_comm.cmd)
			{
				//after read cmd, send latest uart command
				send_packet_to_m2m_uart(TX_TASK_TAG, (const char*)&gm2m_uart_comm, sizeof(M2M_UART_COMMN));
				memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
			}
		}


	}
	if (g_uart_send_task_handle != NULL)
	{
		vTaskDelete(g_uart_send_task_handle); 	//delete task
		g_uart_send_task_handle = NULL;
	}
}

/**
* @brief      This function sends uart event to stm uart via a queue
* @param[in]  struct M2M_UART_COMMN *pm2m_uart_comm
* @retval     void
*/
void UART_Send_Event_To_Queue(void *puart_m2m_data)
{
	M2M_UART_COMMN *pm2m_uart_comm;
	pm2m_uart_comm = (M2M_UART_COMMN *)puart_m2m_data;

	if ((g_uart_send_task_handle != NULL) && (g_send_uart_event_queue != NULL))
	{
		xQueueSend(g_send_uart_event_queue, pm2m_uart_comm, 0);
	}
	return;
}


/**
* @brief M2M UART INIT Function
*        Initialize UART 1 Interface with 115200 baud-rate.
* @param void
* @return void
*/
void init_m2m_uart(void)
{
    const uart_config_t uart_config =
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };


	uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_1, &uart_config);
	uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	DeviceInfo.M2MUartStatus = ESP_UART_READY;
}

void initial_Uart(void)
{
	//factory mode uart connection, need to confirm from Ahmed
	//if(DeviceInfo.DeviceInFactoryMode == DEVICE_IS_IN_FACTORY_MODE)
	{
		init();

		xTaskCreate(rx_task, "uart_rx_task", (DEFAULT_TASK_STACK_SIZE*2), NULL, configMAX_PRIORITIES, NULL);
	}

	//M2M Uart Send Event Queue
	if ((g_send_uart_event_queue = xQueueCreate(UART_CMD_MAX_QUEUE_SIZE, sizeof(M2M_UART_COMMN))) != NULL)
	{
		//M2M UART communication module init
	   	init_m2m_uart();

	   	//create a task to process UART M2M protocol commands
	   	xTaskCreate(m2m_uart_rx_task, "m2m_uart_rx_task", (DEFAULT_TASK_STACK_SIZE*4), NULL, 20, NULL);

	   	//create a task to process UART send event M2M quwuw
		xTaskCreate(Uart_Event_Send_Task, "Uart_Event_Send_Task", (DEFAULT_TASK_STACK_SIZE*2), NULL, tskIDLE_PRIORITY+2, &g_uart_send_task_handle);
	}

}


void Test_AddingConnectionString()
{
    memset(DeviceInfo.ConnectionString, 0, sizeof(DeviceInfo.ConnectionString));
	TRY_STATIC_ASSERT(sizeof(TEST_ConnectionString)<=sizeof(DeviceInfo.ConnectionString),"TEST_ConnectionString Too Large");
	memcpy(DeviceInfo.ConnectionString, TEST_ConnectionString, sizeof(TEST_ConnectionString));
    SetConnectStringInfo();    

}
    
void Test_AddingWifiSSID()
{
    memset(DeviceInfo.Ssid, 0, sizeof(DeviceInfo.Ssid));
	TRY_STATIC_ASSERT(sizeof(TEST_WifiSSID)<=sizeof(DeviceInfo.Ssid),"TEST_WifiSSID Too Large");
    memcpy(DeviceInfo.Ssid, TEST_WifiSSID, sizeof(TEST_WifiSSID));
//    DCI_Update_SSID();
//    GetDimSsid();
    ets_printf("WifiSSID %s\n",DeviceInfo.Ssid);
}

void Test_AddingWifiPWD()
{
    memset(DeviceInfo.PassWord, 0, sizeof(DeviceInfo.PassWord));
	TRY_STATIC_ASSERT(sizeof(TEST_WifiPW)<=sizeof(DeviceInfo.PassWord),"TEST_WifiPW Too Large");
    memcpy(DeviceInfo.PassWord, TEST_WifiPW, sizeof(TEST_WifiPW));
//    DCI_Update_DimPW();
//    GetDimPW();
    //ets_printf("Wifi Password %s\n",DeviceInfo.PassWord);    
    ets_printf("Wifi Password *******\n");
    
}

void Test_RemoveWifiCredential()
{
    ResetDevice(PROVISIONING_RESET, true);
}

void Test_ConnectWiFi()
{
    Factory_WiFi_Connect();
}

void Test_WritingBuildType()
{
    memset(DeviceInfo.BuildType, 0, sizeof(DeviceInfo.BuildType));
	TRY_STATIC_ASSERT(sizeof(TEST_BuildType)<=sizeof(DeviceInfo.BuildType),"TEST_BuildType Too Large");
    memcpy(DeviceInfo.BuildType, TEST_BuildType, sizeof(TEST_BuildType));
    WriteDataToFlash(EEOFFSET_BUILD_TYPE_LABEL,DeviceInfo.BuildType,sizeof(DeviceInfo.BuildType));
    WriteByteToFlash(EEOFFSET_BUILD_TYPE_SAVED_LABEL,BUILD_TYPE_SAVED );
}
    
void Test_WritingNewPartNumber()
{
    memset(DeviceInfo.NewProgramedPartNumber, 0, sizeof(DeviceInfo.NewProgramedPartNumber));
	TRY_STATIC_ASSERT(sizeof(TEST_PartNumber)<=sizeof(DeviceInfo.NewProgramedPartNumber),"TEST_PartNumber Too Large");
    memcpy(DeviceInfo.NewProgramedPartNumber, TEST_PartNumber, sizeof(TEST_PartNumber));
    WriteDataToFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_LABEL,DeviceInfo.NewProgramedPartNumber,sizeof(DeviceInfo.NewProgramedPartNumber));
    WriteByteToFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_SAVED_LABEL,NEW_PROGRAMMED_PART_NO_SAVED );
}
void sblcpSetSecondaryHandle(uint8_t secondaryHandleOnOff)
{

	if(secondaryHandleOnOff == HANDLE_OPEN)
	{
		memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
		prepare_uart_command(&gm2m_uart_comm, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_OPEN, NO_PAYLOAD_2, NO_PAYLOAD_2);
	}
	else if(secondaryHandleOnOff == HANDLE_CLOSE)
	{
		memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
		prepare_uart_command(&gm2m_uart_comm, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_CLOSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
	}
	else if(secondaryHandleOnOff == TOGGLE)
	{
		memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
		prepare_uart_command(&gm2m_uart_comm, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_TOGGLE, NO_PAYLOAD_2, NO_PAYLOAD_2);
	}
	UART_Send_Event_To_Queue((void *)&gm2m_uart_comm);
}

void Test_AddingDPSEndpoint()
{
	memset(DeviceInfo.DpsEndpoint, 0, sizeof(DeviceInfo.DpsEndpoint));
	TRY_STATIC_ASSERT(sizeof(TEST_DpsEndpoint)<=sizeof(DeviceInfo.DpsEndpoint),"TEST_DpsEndpoint Too Large");
	memcpy(DeviceInfo.DpsEndpoint, TEST_DpsEndpoint, sizeof(TEST_DpsEndpoint));
	ets_printf("Updating DCI DpsEndpoint to %s\n", DeviceInfo.DpsEndpoint);
	DCI_UpdateDPSEndpoint();
}

void Test_AddingDPSIDScope()
{
	memset(DeviceInfo.DpsIdScope, 0, sizeof(DeviceInfo.DpsIdScope));
	TRY_STATIC_ASSERT(sizeof(TEST_DpsIdScope)<=sizeof(DeviceInfo.DpsIdScope),"TEST_DpsIdScope Too Large");
	memcpy(DeviceInfo.DpsIdScope, TEST_DpsIdScope, sizeof(TEST_DpsIdScope));
	ets_printf("Updating DCI DpsIdScope to %s\n", DeviceInfo.DpsIdScope);
	DCI_UpdateDPSIDScope();
}

void Test_AddingDPSDeviceRegId()
{
	memset(DeviceInfo.DpsDeviceRegId, 0, sizeof(DeviceInfo.DpsDeviceRegId));
	TRY_STATIC_ASSERT(sizeof(TEST_DpsDeviceRegId)<=sizeof(DeviceInfo.DpsDeviceRegId),"TEST_DpsDeviceRegId Too Large");
	memcpy(DeviceInfo.DpsDeviceRegId, TEST_DpsDeviceRegId, sizeof(TEST_DpsDeviceRegId));
	ets_printf("Updating DCI DpsDeviceRegId to %s\n", DeviceInfo.DpsDeviceRegId);
	DCI_UpdateDPSDeviceRegId();
}

void Test_AddingDPSSymmetricKey()
{
	memset(DeviceInfo.DpsSymmetricKey, 0, sizeof(DeviceInfo.DpsSymmetricKey));
	TRY_STATIC_ASSERT(sizeof(TEST_DpsSymmetricKey)<=sizeof(DeviceInfo.DpsSymmetricKey),"TEST_DpsSymmetricKey Too Large");
	memcpy(DeviceInfo.DpsSymmetricKey, TEST_DpsSymmetricKey, sizeof(TEST_DpsSymmetricKey));
	ets_printf("Updating DCI DpsSymmetricKey to %s\n", DeviceInfo.DpsSymmetricKey);
	DCI_UpdateDPSSymmetricKey();
}

void Test_ReadingDPSEndpoint()
{
	DCI_GetDPSEndpoint();
	ets_printf("After read from DCI: DpsEndpoint is %s\n", DeviceInfo.DpsEndpoint);
}

void Test_ReadingDPSIDScope()
{
	DCI_GetDPSIDScope();
	ets_printf("After read from DCI: DpsIdScope is %s\n", DeviceInfo.DpsIdScope);
}

void Test_ReadingDPSDeviceRegId()
{
	DCI_GetDPSDeviceRegId();
	ets_printf("After read from DCI: DpsDeviceRegId is %s\n", DeviceInfo.DpsDeviceRegId);
}

void Test_ReadingDPSSymmetricKey()
{
	DCI_GetDPSSymmetricKey();
	ets_printf("After read from DCI: DpsSymmetricKey is %s\n", DeviceInfo.DpsSymmetricKey);
}

void Test_AddingDPSMessage()
{
    // Write Test 'full DPS message' contents to DCI/DeviceInfo i.e. parse and set just like UART
	memset(DeviceInfo.DpsConcatenatedMessage, 0, sizeof(DeviceInfo.DpsConcatenatedMessage));
	TRY_STATIC_ASSERT(sizeof(TEST_DpsConcatenatedMessage)<=sizeof(DeviceInfo.DpsConcatenatedMessage),"TEST_DpsConcatenatedMessage Too Large");
	memcpy(DeviceInfo.DpsConcatenatedMessage, TEST_DpsConcatenatedMessage, sizeof(TEST_DpsConcatenatedMessage));
	ets_printf("Updating DCI DPS fields from DeviceInfo.DpsConcatenatedMessage: %s\n", DeviceInfo.DpsConcatenatedMessage);
	SetDPSConcatenatedMessageToIndividualFields();
	DeviceInfo.DpsInfoSaved = true;

}

void Test_ReadingDPSMessage()
{
    // Make sure saved DCI values can be read/concatenated to produce a 'full DPS message' just like UART
	GetDPSConcatenatedMessageFromIndividualFields();
	ets_printf("After read from DCI: DpsDeviceRegId is %s\n", DeviceInfo.DpsDeviceRegId);
	ets_printf("After read from DCI: DpsEndpoint is %s\n", DeviceInfo.DpsEndpoint);
	ets_printf("After read from DCI: DpsIdScope is %s\n", DeviceInfo.DpsIdScope);
	ets_printf("After read from DCI: DpsSymmetricKey is %s\n", DeviceInfo.DpsSymmetricKey);
	ets_printf("Recreated DeviceInfo.DpsConcatenatedMessage is: %s\n", DeviceInfo.DpsConcatenatedMessage);
}

uint8_t get_statusOfBuildType()
{
	return BuildTypeWasProgrammed;
}
