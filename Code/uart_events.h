/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform fan
 *
 * File name        : uart_events.h
 *
 * Author           : Harry Zhang
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 04/20/2021
 *
 ****************************************************************************/

/****************************************************************************
 *                              INCLUDE FILES                               
 ****************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
//#include <string. h>
//#include <stdio. h>

#include "Breaker.h"
#include "LED_Breaker.h"
#include "ErrorLog.h"


/****************************************************************************
 *                              General definitions                               
 ****************************************************************************/


#define UART_DIMMER_CONNECT     0x01
#define UART_FSC_CONNECT        0x02
#define UART_SWITCH_CONNECT     0x03
#define UART_RECEPTECAL_CONNECT 0x04
#define UART_BREAKER_CONNECT    0x05

#ifdef _OLD_APP_NO_BREAKER_
	#define UART_DEVICE_TYPE        UART_DIMMER_CONNECT
#else
	#define UART_DEVICE_TYPE        UART_BREAKER_CONNECT
#endif

//Messages
#define UART_CONNECT_MESSAGE                        0x01
#define UART_CONNECT_REPLY_MESSAGE                  0x81

#define UART_SEND_CONNECTION_STRING_MESSAGE         0x02
#define UART_SEND_CONNECTION_STRING_REPLY_MESSAGE   0x82

#define UART_GET_CONNECTION_STRING_MESSAGE          0x03
#define UART_GET_CONNECTION_STRING_REPLY_MESSAGE    0x83

#define UART_GET_FIRMWARE_VERSION_MESSAGE           0x04
#define UART_GET_FIRMWARE_VERSION_REPLY_MESSAGE     0x84

#define UART_GET_DEVICE_ID_MESSAGE                  0x05
#define UART_GET_DEVICE_ID_REPLY_MESSAGE            0x85

#define UART_GET_SHARED_ACCESS_KEY_MESSAGE          0x06
#define UART_GET_SHARED_ACCESS_KEY_REPLY_MESSAGE    0x86

#define UART_GET_DEVICE_PROFILE_TYPE_MESSAGE        0x07
#define UART_GET_DEVICE_PROFILE_TYPE_REPLY_MESSAGE  0x87

#define UART_LOCK_DEVICE_MESSAGE          		    0x08
#define UART_LOCK_DEVICE_REPLY_MESSAGE     			0x88

#define UART_GET_WIFI_STATUS_MESSAGE           		0x09
#define UART_GET_WIFI_STATUS_REPLY_MESSAGE     		0x89

#define UART_SET_SSID_MESSAGE           			0x0A
#define UART_SET_SSID_REPLY_MESSAGE     			0x8A

#define UART_GET_SSID_MESSAGE           			0x0B
#define UART_GET_SSID_REPLY_MESSAGE     			0x8B

#define UART_SET_WIFI_PASSWORD_MESSAGE           	0x0C
#define UART_SET_WIFI_PASSWORD_REPLY_MESSAGE     	0x8C

#define UART_GET_WIFI_PASSWORD_MESSAGE           	0x0D
#define UART_GET_WIFI_PASSWORD_REPLY_MESSAGE     	0x8D

#define UART_SET_QR_INFO_MESSAGE           			0x0E
#define UART_SET_QR_INFO_REPLY_MESSAGE     			0x8E

#define UART_GET_QR_INFO_MESSAGE           			0x0F
#define UART_GET_QR_INFO_REPLY_MESSAGE     			0x8F

#define UART_GET_MAC_ADDRESS_MESSAGE           		0x10
#define UART_GET_MAC_ADDRESS_REPLY_MESSAGE     		0x90

#define UART_CONNECT_WIFI_MESSAGE           		0x11
#define UART_CONNECT_WIFI_REPLY_MESSAGE     		0x91

#define UART_DISCONNECT_WIFI_MESSAGE           		0x12
#define UART_DISCONNECT_WIFI_REPLY_MESSAGE     		0x92

#define UART_CHECK_IOT_STATUS_MESSAGE           	0x13
#define UART_CHECK_IOT_STATUS_REPLY_MESSAGE     	0x93

#define UART_ERASE_WIFI_CREDENTIAL_MESSAGE          0x14
#define UART_ERASE_WIFI_CREDENTIAL_REPLY_MESSAGE    0x94

#define UART_CONNECT_IOT_MESSAGE           			0x15
#define UART_CONNECT_IOT_REPLY_MESSAGE     			0x95

#define UART_RESET_DEVICE_MESSAGE           		0x16
#define UART_RESET_DEVICE_REPLY_MESSAGE     		0x96


#define UART_SET_BUILD_TYPE_MESSAGE            		0x17
#define UART_SET_BUILD_TYPE_REPLY_MESSAGE      		0x97

#define UART_GET_BUILD_TYPE_MESSAGE            		0x18
#define UART_GET_BUILD_TYPE_REPLY_MESSAGE     		0x98

#define UART_SET_PROGRAMMED_PART_NUMBER_MESSAGE 	    0x19
#define UART_SET_PROGRAMMED_PART_NUMBER_REPLY_MESSAGE   0x99

#define UART_GET_PROGRAMMED_PART_NUMBER_MESSAGE       	0x1A
#define UART_GET_PROGRAMMED_PART_NUMBER_REPLY_MESSAGE 	0x9A



#define UART_CRC_ERROR_REPLY_MESSAGE         		0xF0
#define UART_COMMAND_ERROR_REPLY_MESSAGE     		0xF1

#define NO_PAYLOAD 0x00
#define ESP_UART_READY 0x01

#define DEFAULT_TASK_STACK_SIZE                     1024
/******************************************************************************/

#define START_BYTE   0x7e
#define END_BYTE     0x7f

#define NO_PAYLOAD_2 0
#define TXD_PIN (GPIO_NUM_32)
#define RXD_PIN (GPIO_NUM_35)

typedef enum {
	SWITCH_1_PRESS = 0x01,//change name according to button functionality
	SWITCH_2_PRESS
}SWITCH_PRESS;


typedef struct {
	uint8_t start_flag; //HEX start of packet end of packet flag
	uint8_t cmd;        //handler side what to do.
	uint8_t len;        // payload length
	uint8_t payload_1;   //command data param 1
	uint8_t payload_2;   //command data param 2
	uint8_t payload_3;   //command data param 3
	uint8_t crc;        //crc function to implement for data validation
					             // (8/16 bitcrc check timing, for max bytes data)
	uint8_t end_flag;   //end of frame flag
}M2M_UART_COMMN;


typedef enum {
	//review
	//button events needs to be updated and also
	//communicated
	//LED Pattern : color, rate and duration of LED blinking pattern (Wifi status, provision)
	// when to start and stop the LED based on events
	BREAKER_FAULT_STATE       =    0x20,
	BREAKER_OPEN_CLOSE,
	COMM_MCU_ACK,
	PROTECTION_MCU_ACK,
	READ_ESP_UART_STATUS,
	BUTTON_PRESS_1_STATUS,
	SWITCH2_PRESS_PROVISION,//rename as per button funcitonality
	READ_COMMUNICATION_MCU_CMD,
	READ_WIFI_STATUS,//read wifi status connected to AP or open to connect or no AP found or no internet on AP
	READ_ESP_STATUS, //factory reset or first boot up
	LED_COLOR_BLINK_RATE,
	STM32_OTA_MODE,
	STM32_OTA_START,
	STM32_OTA_DATA,
	STM32_OTA_END,
	BREAKER_STATUS,
	ESP_FACTORY_RESET = 0xFE,
	ERROR_RESPONSE            =    0xFF,
}UART_COMMANDS;

typedef enum {
	SECONDARY_CONTACTS_OPEN = 0x01,
	SECONDARY_CONTACTS_CLOSE,
	SECONDARY_CONTACTS_TOGGLE
}BREAKER_SECONDARY_CONTACT_STATE;

typedef enum{
	BREAKER_PROVISION_DONE = 0x61,

}WIFI_STATUS;

typedef enum{
	FACTORY_RESET = 0x01,
	STANDARD_RESET = 0x02,

}RESET_COMMAND_TYPE;

typedef enum {
	INVALID_CMD_ERROR_RESPONSE = 0x51,
	CRC_ERROR_RESPONSE,
	WIFI_CONNECT_AP_ERROR_RESPONSE, //for LED pattern

}ERROR_RESPONSE_TYPE;

/******************************************************************************/





/****************************************************************************
 *                              Functoion's prototyps      
 ****************************************************************************/

int send_packet_to_m2m_uart(const char* logName, const char* data, const int len);
int process_uart_packet(M2M_UART_COMMN *pm2m_uart_comm);
int prepare_uart_packet(M2M_UART_COMMN *pm2m_uart_comm);
int process_breaker_fault_state(M2M_UART_COMMN *pm2m_uart_comm);
int prepare_uart_command(M2M_UART_COMMN *pm2m_uart_comm, UART_COMMANDS uart_cmd, uint8_t payload_1, uint8_t payload_2, uint8_t payload_3);
void process_breaker_status(M2M_UART_COMMN *pm2m_uart_comm);

void init_m2m_uart(void);

#ifdef __cplusplus
extern "C"
{
#endif

    
    
    
#ifdef __cplusplus
}
#endif




