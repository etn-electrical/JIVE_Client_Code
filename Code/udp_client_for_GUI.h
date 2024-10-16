/* ****************************************************************************************************
 *
 * 		Connected Solutions @ Eaton (Moon Township, PA)
 *
 * 		Author	: Jonathan Tan (jonathantan@eaton.com)
 * 		Date	: 6/14/2023
 *
 * ****************************************************************************************************
 *
 * 		udp_client_for_GUI.h
 *
 * 		Purpose	: This file is intended to use WITH the SBLCP Local Terminal windows application (
 * 				  https://github.com/etn-electrical/SBLCP_local_terminal_GUI).
 * 				  How this file works is, it creates an API to which the SBLCP Local Terminal app will
 * 				  use. The idea of the API, in short, is key presses. In PuTTy, if, for example,
 * 				  char '4' is pressed, this file will send out a GET_DEVICE_STATUS command, '1'
 * 				  is GET_NEXT_UDP command, etc.
 * 				  The SBLCP Local Terminal will do the key "pressing". Checkout the main function
 * 				  udp_client_txrx_task to see exactly how it works, it should be fairly straight
 * 				  forward to understand with my naming scheme.
 *
 * 		Docs	: https://confluence-prod.tcc.etn.com/pages/viewpage.action?pageId=299547479#DesktopSoftwareforCommunicatingwithSB2usingSBLCP.-Backendstuff(Ignoreifyouarejustusingthesoftware):
 *
 *****************************************************************************************************/

#ifndef MAIN_UDP_CLIENT_FOR_GUI_H_
#define MAIN_UDP_CLIENT_FOR_GUI_H_

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>




#define EXTRACT_DATA_1_BYTE(msg, offset)   		(msg[offset])
#define EXTRACT_DATA_2_BYTE(msg, offset)    	(msg[offset] << 8 | msg[offset + 1])
#define EXTRACT_REV_2_BYTE(msg, offset)    		(msg[offset + 1] << 8 | msg[offset])
#define EXTRACT_REV_DATA_4_BYTE(msg, offset)	((msg[offset] << 24) | (msg[offset + 1] << 16) | (msg[offset + 2] << 8) | msg[offset + 3])
#define EXTRACT_DATA_4_BYTE(msg, offset)		((msg[offset + 3] << 24) | (msg[offset + 2] << 16) | (msg[offset + 1] << 8) | msg[offset])

#define GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER	0x0000
#define GET_DEVICE_STATUS						0x00FF
#define GET_REMOTE_HANDLE_POSISTION				0x0100
#define GET_METER_TELEMETRT_DATA				0x0200
#define SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER	0x8000
#define SET_REMOTE_HANDLE_POSISTION				0x8100
#define SET_LED_TO_USER_DEFINED_COLOUR			0x8300
#define SET_BREAKER_EOL_CALIB_PARAM             0x8400
#define GET_BREAKER_EOL_CALIB_PARAM             0x8500
#define SET_MANUFACTURING_MODE_ON				0x9100
#define SET_MANUFACTURING_MODE_OFF				0x9200
#define RESET_ENERGY							0x8600
#define GET_PRIMARY_HANDLE_STATUS				0x0300
#define GET_METER_TELEMETRY_DATA				0x0200
#define GET_TRIP_CURVE							0x0600
#define GET_MAC_ADDRESS							0x0500
#define SET_MANUFACTURING_RESET					0x8700
#define SET_DEVICE_LOCK							0x8200
#define IDENTIFY_ME								0x0400


#define SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER		0x0000
#define SEND_GET_DEVICE_STATUS							0xFF00
#define SEND_GET_REMOTE_HANDLE_POSISTION				0x0001
#define SEND_GET_METER_TELEMETRT_DATA					0x0002
#define SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER		0x0080
#define SEND_SET_REMOTE_HANDLE_POSISTION				0x0081
#define SEND_SET_LED_TO_USER_DEFINED_COLOUR				0x0083
#define SEND_SET_BREAKER_EOL_CALIB_PARAM             	0x0084
#define SEND_GET_BREAKER_EOL_CALIB_PARAM				0x0085
#define SEND_SET_MANUFACTURING_MODE_ON					0x0091
#define SEND_SET_MANUFACTURING_MODE_OFF					0x0092
#define SEND_RESET_ENERGY								0x0086
#define SEND_GET_PRIMARY_HANDLE_STATUS					0x0003
#define SEND_GET_METER_TELEMETRY_DATA					0x0002
#define SEND_GET_TRIP_CURVE								0x0006
#define SEND_GET_MAC_ADDRESS							0x0005
#define SEND_SET_MANUFACTURING_RESET					0x0087
#define SEND_SET_DEVICE_LOCK							0x0082
#define SEND_IDENTIFY_ME								0x0004

#define HEADER_OFFSET			10
//#define HASH_SIZE				32
#define STAR_OF_MESSAGE			0x4d4e5445 //0x45544e4d
#define SERIAL_NO_LENGTH		16
#define TRIP_LOG_SIZE				100

// From ADE9000.h
#define 	ADE9000_AIGAIN		0xFDFE05B6	 		//Phase A current gain adjust
#define 	ADE9000_AVGAIN 		0X1C369D7			//Phase A voltage gain adjust
#define 	ADE9000_AIRMSOS 	0xFFFFFFEA			//Phase A current offset adjust
#define 	ADE9000_AVRMSOS 	0x00000004			//Phase A voltage offset adjust
#define 	ADE9000_APHCAL0 	0xCA4AD				//Phase A phase compensation
#define 	ADE9000_APGAIN 		0x2064
#define 	ADE9000_BPGAIN 		0xFFFE172E
#define 	ADE9000_BIGAIN 		0xF223167D			//Phase B current gain adjust
#define 	ADE9000_BVGAIN 		0X1CCA459			//Phase B voltage gain adjust
#define 	ADE9000_BIRMSOS 	0xFFFFFFF6			//Phase B current offset adjust
#define 	ADE9000_BVRMSOS 	0x00000004			//Phase B voltage offset adjust
#define 	ADE9000_BPHCAL0 	0xC9A0F				//Phase B phase compensation


typedef struct {
	char ipAddress[128];
	char serial_number[SERIAL_NO_LENGTH];
	int sequenceNumber;
} breakerIdentifier_st;


typedef union
{
	uint8_t seq_num_arr[4];
	uint32_t seq_num;
}seq_num_conversion;

uint8_t MacAddress[20];

//typedef struct __attribute__((__packed__))
//{
//	uint8_t meter_update_no;
//	uint16_t period;
//
//	float phase_A_line_frequency;
//	float phase_A_rms_voltage;
//	float phase_A_rms_current;
//	float phase_A_power_factor;
//	float phase_A_active_power;
//	float phase_A_reactive_power;
//	float phase_A_app_power;
//	double phase_A_active_energy;
//	double phase_A_reactive_energy;
//	double phase_A_app_energy;
//	double phase_A_reverse_active_energy;
//	double phase_A_reverse_reactive_energy;
//	double phase_A_reverse_app_energy;
//
//	float phase_B_line_frequency;
//	float phase_B_rms_voltage;
//	float phase_B_rms_current;
//	float phase_B_power_factor;
//	float phase_B_active_power;
//	float phase_B_reactive_power;
//	float phase_B_app_power;
//	double phase_B_active_energy;
//	double phase_B_reactive_energy;
//	double phase_B_app_energy;
//	double phase_B_reverse_active_energy;
//	double phase_B_reverse_reactive_energy;
//	double phase_B_reverse_app_energy;
//
//	uint64_t phase_A_Q1_act_energy;
//	uint64_t phase_A_Q2_act_energy;
//	uint64_t phase_A_Q3_act_energy;
//	uint64_t phase_A_Q4_act_energy;
//	uint64_t phase_A_Q5_act_energy;
//	uint64_t phase_A_Q1_reactive_energy;
//	uint64_t phase_A_Q2_reactive_energy;
//	uint64_t phase_A_Q3_reactive_energy;
//	uint64_t phase_A_Q4_reactive_energy;
//	uint64_t phase_A_Q5_reactive_energy;
//	uint64_t phase_A_Q1_app_energy;
//	uint64_t phase_A_Q2_app_energy;
//	uint64_t phase_A_Q3_app_energy;
//	uint64_t phase_A_Q4_app_energy;
//	uint64_t phase_A_Q5_app_energy;
//
//	uint64_t phase_B_Q1_act_energy;
//	uint64_t phase_B_Q2_act_energy;
//	uint64_t phase_B_Q3_act_energy;
//	uint64_t phase_B_Q4_act_energy;
//	uint64_t phase_B_Q5_act_energy;
//	uint64_t phase_B_Q1_reactive_energy;
//	uint64_t phase_B_Q2_reactive_energy;
//	uint64_t phase_B_Q3_reactive_energy;
//	uint64_t phase_B_Q4_reactive_energy;
//	uint64_t phase_B_Q5_reactive_energy;
//	uint64_t phase_B_Q1_app_energy;
//	uint64_t phase_B_Q2_app_energy;
//	uint64_t phase_B_Q3_app_energy;
//	uint64_t phase_B_Q4_app_energy;
//	uint64_t phase_B_Q5_app_energy;
//	float phase_to_phase_voltage;
//}meterTelemetryData_st;
//
//#define AMAZON_DEMO
//
//typedef struct __attribute__((__packed__))
//{
//	uint8_t breaker_state;
//	uint8_t primary_handle_status;
//	uint8_t path_status;
//	meterTelemetryData_st telemetry_data;
//#if defined(AMAZON_DEMO)
//	uint8_t InputScenario;
//#endif
//}breakerStatus_st;
//
//typedef struct
//{
//	uint8_t led_colour;
//	uint8_t blinking_time_interval;
//	uint8_t blinking_time_expire;
//	uint8_t start_blinking_leds_flag;
//}setLedColour_st;
//
//typedef struct __attribute__((__packed__))
//{
//	uint32_t seq_number;
//	char serial_number[SERIAL_NO_LENGTH];
//	uint32_t version;
//	uint32_t nonce;
//}getSeqNumberMesg_st;
//
//typedef struct __attribute__((__packed__))
//{
//	uint8_t responce;
//	uint8_t handle_posistion;
//}setHandlePosition_st;
//
//typedef struct	// From rahul
//{
//	uint32_t avGain;
//	uint32_t aiGain;
//	uint32_t aiRmsOs;
//	uint32_t apGain;
//	uint32_t aphCal_0;
//
//	uint32_t bvGain;
//	uint32_t biGain;
//	uint32_t biRmsOs;
//	uint32_t bpGain;
//	uint32_t bphCal_0;
//} BreakerEOLCalibration_st;
typedef struct
{
    uint8_t ErrorLog;
    bool CloudUpdated;
    uint32_t EpochTime;
}tripLog_st;
typedef struct
{
	tripLog_st logs[TRIP_LOG_SIZE];
	uint8_t index;
}ErrorLog_st;

typedef enum
{
	get_seq_number = 0,
	Manufactring_mode_on,
	get_remote_handle_status,
	get_device_status,
	set_remote_handle,
	set_led_to_user_setting,
	set_metro_calibration,
	get_metro_calibration,
	Manufactring_mode_off,
	set_next_sequence_number
}sendingCommands_e;

#define ConnectionStringLength 	170
#define HostNameLength 			55
#define DeviceIdLength 			45
#define SharedAccessKeyLength 	50
#define CRYPTO_HASH_LENGTH		32


// Because I (jonathan) is leaving this Friday (8/11/2023), after implementing
// this hashing thingy I won't have a huge opportunity to test it, if in case
// it breaks, comment out this line and you will be reverted to the old version
// aka w/o hashing
#define HASHING_IMPLEMENTED



#ifdef __cplusplus
extern "C"
{
#endif

void udp_client_txrx_task(void* nothing);
void app_main_client(void);
void getWifiCredentialsFromGUIandConnectToWifi();
int getWifiCredentialsFromGUI();
int connectToWifi();
void sendWifiErrorJSON(int SSID_Error, int Pass_Error, int Send_Error, int Other_Error);
int pingAllBreakersOnTheNetwork();
void selectWhichIpAddressToTalkTo(int index);
int sendHandshakeMessage();
int send_SBLCP_message(uint16_t givenMessageCode, void* givenMessageBody);
void modifyHashToSendArrayUsingGivenPsendMsg(uint8_t* psendMsg, int messageLength_B, int messageLendth_wo_hash_B);
int send_SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER_message();
int send_SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER_message();
int send_SEND_GET_REMOTE_HANDLE_POSISTION_message();
int send_SEND_GET_PRIMARY_HANDLE_STATUS_message();
int send_SEND_SET_REMOTE_HANDLE_POSISTION_OPEN_message();
int send_SEND_SET_REMOTE_HANDLE_POSISTION_CLOSE_message();
int send_SEND_SET_REMOTE_HANDLE_POSISTION_TOGGLE_message();
int send_SEND_SET_MANUFACTURING_MODE_ON_message();
int send_SEND_SET_MANUFACTURING_MODE_OFF_message();
int send_SEND_GET_DEVICE_STATUS_message();
int send_SEND_GET_METER_TELEMETRY_DATA_message();
int send_SEND_SET_LED_TO_USER_DEFINED_COLOUR_message();
int send_MIKE_ENDURANCE_TEST_START();
int send_MIKE_ENDURANCE_TEST_STOP();
int send_SEND_GET_BREAKER_EOL_CALIB_PARAM_message();
int send_SEND_SET_BREAKER_EOL_CALIB_PARAM_message();
int send_SEND_RESET_ENERGY_message();
int send_customMessage();
int send_SEND_GET_MAC_ADDRESS();
int send_SEND_GET_TRIP_CURVE();
int send_SEND_MANUFACTRUNG_RESET();
int send_SEND_DEVICE_LOCK();
int send_SEND_IDENTIFY_ME();

int listenToBreakerResponseAndPrintResponse();
void makeSureHostIPAddressIsRight(char* givenIPAddress);
int verifyStingRead();
uint8_t hexStringToUint8(char* givenString);
uint16_t stringToUint16(char* givenString);
uint16_t hexStringToUint16(char* givenString);
uint32_t stringToUint32(char* givenString);
uint32_t hexStringToUint32(char* givenString);
int extractCalibParamStringIntoStruct();
uint32_t extractSetNextUDPSequenceNumberStringIntoInt();
int listenAndExtractCustomMessageParametersInString();
uint32_t bigEndianToLittleEndian32bits(uint32_t num);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_UDP_CLIENT_FOR_GUI_H_ */
