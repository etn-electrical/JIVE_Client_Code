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


extern DRAM_ATTR Device_Info_str DeviceInfo;

static bool BuildTypeWasProgrammed = false;

bool guart_recvd_err = false;
uint8_t buf[4] = { 0xFA, 0x11, 0x28, 0x33 };

M2M_UART_COMMN gm2m_uart_comm = {0};

M2M_UART_COMMN gm2m_uart_comm_ss_onoff = {0};

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
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
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
							inlen = ConnectionStringLengthReceived;
							uint8_t *inbuf = &tx_data[4];
                            GetConnectStringInfo();
							//SetConnectStringInfo();
							tx_data[0]=UART_DEVICE_TYPE;
							tx_data[1]=UART_GET_CONNECTION_STRING_REPLY_MESSAGE;
							tx_data[2]=0x00;
							tx_data[3]= inlen;
							memcpy (inbuf,(char *)&DeviceInfo.ConnectionString, ConnectionStringLength);
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
	//uint8_t packet_data = (START_BYTE + uart_cmd + sizeof(M2M_UART_COMMN) + payload_1 + payload_2 + payload_3 + END_BYTE);
	pm2m_uart_comm->start_flag      = START_BYTE;
	pm2m_uart_comm->cmd             = uart_cmd;
	pm2m_uart_comm->len             = sizeof(M2M_UART_COMMN);
	pm2m_uart_comm->payload_1       = payload_1;
	pm2m_uart_comm->payload_2       = payload_2;
	pm2m_uart_comm->payload_3       = payload_3;
	pm2m_uart_comm->crc             = CRC8((const uint8_t *)pm2m_uart_comm, (pm2m_uart_comm->len-2));
	pm2m_uart_comm->end_flag        = END_BYTE;

	return 0;
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
//	if(replyStatus.timout_command == true)
//	{
		replyStatusInfo.status_updated = true;
//	}
	return 0;
}

void process_breaker_status(M2M_UART_COMMN *pm2m_uart_comm)
{
	//printf("process_breaker_status\n");

	DeviceInfo.PrimaryContactState = pm2m_uart_comm->payload_1;
	DeviceInfo.SecondaryContactState = pm2m_uart_comm->payload_2;
	DeviceInfo.PathStatus = pm2m_uart_comm->payload_3;

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
			process_breaker_fault_state(pm2m_uart_comm);
			break;

		case BREAKER_OPEN_CLOSE:
			process_open_close(pm2m_uart_comm);
			break;

		case COMM_MCU_ACK:
			break;

		case PROTECTION_MCU_ACK:
			break;

		case ESP_FACTORY_RESET:
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
			process_button_press_1(pm2m_uart_comm);
		break;

		case BREAKER_STATUS:
			process_breaker_status(pm2m_uart_comm);
		break;

		case SWITCH2_PRESS_PROVISION:
			process_switch2_press_provision(pm2m_uart_comm);
		break;

		case READ_COMMUNICATION_MCU_CMD:
		{
			if(gm2m_uart_comm_ss_onoff.cmd)
			{
				send_packet_to_m2m_uart(TX_TASK_TAG, (const char*)&gm2m_uart_comm_ss_onoff, sizeof(M2M_UART_COMMN));
				memset(&gm2m_uart_comm_ss_onoff, 0x00, sizeof(gm2m_uart_comm_ss_onoff));
			}
			else
			{
				if(gm2m_uart_comm.cmd)
				{
					//after read cmd, send latest uart command
					send_packet_to_m2m_uart(TX_TASK_TAG, (const char*)&gm2m_uart_comm, sizeof(M2M_UART_COMMN));
					memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
				}
			}
		}
		break;

		case READ_WIFI_STATUS:
			break;

		case READ_ESP_STATUS:
			break;

		default:
		{
			//Error response to user/cloud or if LED is finalized then use LED to show error
			//if error response received update a flag or use a queue to always send a command to
			//protection MCU
			guart_recvd_err =  true;
			//prepare uart command for crc error response
			prepare_uart_command(pm2m_uart_comm, ERROR_RESPONSE, INVALID_CMD_ERROR_RESPONSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
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
                packet_data = (m2m_uart_comm.start_flag + m2m_uart_comm.cmd + m2m_uart_comm.len + m2m_uart_comm.payload_1 + m2m_uart_comm.end_flag);
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
					        process_uart_packet(&m2m_uart_comm);
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

	//need to discuss, if UART communication needs to be up before the device is
	//provisioned at client location
    if(DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
    	init_m2m_uart();
    	//create a task to process UART M2M Protocol commands
    	xTaskCreate(m2m_uart_rx_task, "m2m_uart_rx_task", (DEFAULT_TASK_STACK_SIZE*4), NULL, configMAX_PRIORITIES, NULL);
    }
}


void Test_AddingConnectionString()
{
    char ValidConnectionString[]= //"HostName=EatonCRDSWDeviceIothub1.azure-devices.net;DeviceId=6ab907ed-31b7-4a9b-a548-7b86afecd7da;SharedAccessKey=G73Zxjg/+9XvLZOLFQKuY7mivER1dwKo7NYtdSs7B94=";
                              "HostName=EatonProdCRDSWDeviceIothub1.azure-devices.net;DeviceId=71595f43-2009-403b-99f2-211e5d71b1bc;SharedAccessKey=oi+AwisDLztUJRUYA8icsy1E+USdUV/fy2lFCf+HZb0=";
    
    memset ((char *)&DeviceInfo.ConnectionString,0, sizeof (DeviceInfo.ConnectionString));
    memcpy ((char *)&DeviceInfo.ConnectionString, (char *)&ValidConnectionString,sizeof (ValidConnectionString));
    SetConnectStringInfo();    

}
    
void Test_AddingWifiSSID()
{
    char WifiSSID[] = ""; //TP-LINK_2.4GHz_EC03D5
    //Ahmed's phone
    memset ((char *)&DeviceInfo.Ssid,0, sizeof (DeviceInfo.Ssid));
    memcpy ((char *)&DeviceInfo.Ssid, (char *)&WifiSSID, sizeof(WifiSSID));
//    DCI_Update_SSID();
//    GetDeviceSsid();
    ets_printf("WifiSSID %s\n",DeviceInfo.Ssid);
}
void Test_AddingWifiPWD()
{
    char WifiPW[] = "";
    //Letmein1
    memset ((char *)&DeviceInfo.PassWord,0, sizeof (DeviceInfo.PassWord));
    memcpy ((char *)&DeviceInfo.PassWord, WifiPW, sizeof(WifiPW));
//    DCI_Update_DimPW();
//    GetDimPW();
    //ets_printf("Wifi Password %s\n",DeviceInfo.PassWord);    
    
    
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
	char Buff[]= "AhmedBuild";
    memset ((char *)&DeviceInfo.BuildType,0, sizeof (DeviceInfo.BuildType));
    memcpy ((char *)&DeviceInfo.BuildType,(char *)&Buff, sizeof(Buff));
    WriteDataToFlash(EEOFFSET_BUILD_TYPE_LABEL,DeviceInfo.BuildType,sizeof(DeviceInfo.BuildType));
    WriteByteToFlash(EEOFFSET_BUILD_TYPE_SAVED_LABEL,BUILD_TYPE_SAVED );
}
    
void Test_WritingNewPartNumber()
{
	char Buff[]= "AhmedDimmer";
    memset ((char *)&DeviceInfo.NewProgramedPartNumber,0, sizeof (DeviceInfo.NewProgramedPartNumber));
    memcpy ((char *)&DeviceInfo.NewProgramedPartNumber,(char *)&Buff, sizeof(Buff));
    WriteDataToFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_LABEL,DeviceInfo.NewProgramedPartNumber,sizeof(DeviceInfo.NewProgramedPartNumber));
    WriteByteToFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_SAVED_LABEL,NEW_PROGRAMMED_PART_NO_SAVED );
}
void sblcpSetSecondaryHandle(uint8_t secondaryHandleOnOff)
{
	printf("secondaryHandleOnOff %d \n", secondaryHandleOnOff);
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
}
