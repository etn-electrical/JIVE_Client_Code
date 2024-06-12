/****************************************************************************
 *
 *                      Eaton India Innovation Center()
 *
 ****************************************************************************
 * Description      :
 *
 * File name        : sblcp.h
 *
 * Author           : Nitin Bhosale
 *
 * Last Changed By  : $Author:  $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/14/2023
 *
 ****************************************************************************/

#ifndef CODE_SBLCP_SBLCP_H_
#define CODE_SBLCP_SBLCP_H_

#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
# include "cryptoApi.h"

//--------------------------------------------------------------------------------------------------
//                    Internal Definitions
//--------------------------------------------------------------------------------------------------

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
#include "SBLCP.h"
#include "Breaker.h"
//#include "uart_events.h"

#define EXTRACT_DATA_1_BYTE(msg, offset)    (msg[offset])
#define EXTRACT_DATA_2_BYTE(msg, offset)    (msg[offset] << 8 | msg[offset + 1])
#define EXTRACT_DATA_4_BYTE(msg, offset)	((msg[offset] << 24) | (msg[offset + 1] << 16) | (msg[offset + 2] << 8) | msg[offset + 3])
//#define EXTRACT_DATA_4_BYTE(msg, offset)	((msg[offset] | (msg[offset + 1] << 8) | (msg[offset + 2] << 16) | msg[offset + 3] << 24))

#define SERIAL_NO_LENGTH		16
#define INVALID_LED_DURATION	10737418
#define SBLCP_ERROR             (-1)
#define SBLCP_SUCCESS           (1)

#define SBLCP_DEBUG				1

typedef struct __attribute__((__packed__))
{
	uint32_t start_byte;
	uint32_t seq_number;
	uint16_t mesg_code;
}sblcpHeader_st;

typedef struct __attribute__((__packed__))
{
	sblcpHeader_st sblcp_header;
	uint8_t *message_data;
	uint8_t sblcp_hash[CRYPTO_HASH_LENGTH];
}sblcpResponce_st;

typedef struct __attribute__((__packed__))
{
	uint32_t seq_number;
	char serial_number[SERIAL_NO_LENGTH];
	uint32_t version;
	uint32_t nonce;
}getSeqNumberMesg_st;

typedef struct __attribute__((__packed__))
{
	sblcpHeader_st header;
	uint8_t responce;
	uint8_t hash[32];
}responceMesg_st;

typedef struct __attribute__((__packed__))
{
	uint8_t responce;
	uint8_t handle_posistion;
}setHandlePosition_st;

typedef struct __attribute__((__packed__))
{
	uint8_t meter_update_no;
	uint16_t period;

	float phase_A_line_frequency;
	float phase_A_rms_voltage;
	float phase_A_rms_current;
	float phase_A_power_factor;
	float phase_A_active_power;
	float phase_A_reactive_power;
	float phase_A_app_power;
	double phase_A_active_energy;
	double phase_A_reactive_energy;
	double phase_A_app_energy;
	double phase_A_reverse_active_energy;
	double phase_A_reverse_reactive_energy;
	double phase_A_reverse_app_energy;

	float phase_B_line_frequency;
	float phase_B_rms_voltage;
	float phase_B_rms_current;
	float phase_B_power_factor;
	float phase_B_active_power;
	float phase_B_reactive_power;
	float phase_B_app_power;
	double phase_B_active_energy;
	double phase_B_reactive_energy;
	double phase_B_app_energy;
	double phase_B_reverse_active_energy;
	double phase_B_reverse_reactive_energy;
	double phase_B_reverse_app_energy;

	uint64_t phase_A_Q1_act_energy;
	uint64_t phase_A_Q2_act_energy;
	uint64_t phase_A_Q3_act_energy;
	uint64_t phase_A_Q4_act_energy;
	uint64_t phase_A_Q5_act_energy;
	uint64_t phase_A_Q1_reactive_energy;
	uint64_t phase_A_Q2_reactive_energy;
	uint64_t phase_A_Q3_reactive_energy;
	uint64_t phase_A_Q4_reactive_energy;
	uint64_t phase_A_Q5_reactive_energy;
	uint64_t phase_A_Q1_app_energy;
	uint64_t phase_A_Q2_app_energy;
	uint64_t phase_A_Q3_app_energy;
	uint64_t phase_A_Q4_app_energy;
	uint64_t phase_A_Q5_app_energy;

	uint64_t phase_B_Q1_act_energy;
	uint64_t phase_B_Q2_act_energy;
	uint64_t phase_B_Q3_act_energy;
	uint64_t phase_B_Q4_act_energy;
	uint64_t phase_B_Q5_act_energy;
	uint64_t phase_B_Q1_reactive_energy;
	uint64_t phase_B_Q2_reactive_energy;
	uint64_t phase_B_Q3_reactive_energy;
	uint64_t phase_B_Q4_reactive_energy;
	uint64_t phase_B_Q5_reactive_energy;
	uint64_t phase_B_Q1_app_energy;
	uint64_t phase_B_Q2_app_energy;
	uint64_t phase_B_Q3_app_energy;
	uint64_t phase_B_Q4_app_energy;
	uint64_t phase_B_Q5_app_energy;
	float phase_to_phase_voltage;
}meterTelemetryData_st;

typedef struct __attribute__((__packed__))
{
	uint8_t breaker_state;
	uint8_t primary_handle_status;
	uint8_t path_status;
	meterTelemetryData_st telemetry_data;
}breakerStatus_st;

typedef struct __attribute__((__packed__))
{
	uint8_t led_colour;
	uint8_t blinking_time_interval;
	uint8_t blinking_time_expire;
	uint8_t start_blinking_leds_flag;
}setLedColour_st;

//typedef struct{
//	psCipherType_       etype;
//	const unsigned char udp_key[CRYPTO_HASH_LENGTH];
//	psSize_t            udp_keyLen;
//	unsigned char       message_data[255];//need to use #define
//	uint32_t            msgdata_len;
//	unsigned char       cryptop_hmac_hash[MAX_HASHLEN];
//}CRYPTOGRAPHIC_HMAC_HASH;


typedef struct
{
	uint32_t avGain;
	uint32_t aiGain;
	uint32_t aiRmsOs;
	uint32_t apGain;
	uint32_t aphCal_0;

	uint32_t bvGain;
	uint32_t biGain;
	uint32_t biRmsOs;
	uint32_t bpGain;
	uint32_t bphCal_0;
}BreakerEOLCalibration_st;

typedef struct
{
	uint32_t macAddress;
}BreakerMacAddress_st;

typedef enum
{
	ACK = 0,		//Command executed and Next Expected Sequence Number updated.
	NACK,			//Rate Limiting has prevented the message from succeeding. Try again later.
	BAD_RESPONCE,	//Sequence Number is in the invalid range
	DONT_CARE,
}responceAckNac_e;

typedef enum
{
	HANDLE_OPEN = 0,	//remote handle open
	HANDLE_CLOSE,		//remote handle close
	TOGGLE,				//toggle remote handle posistion
}handlePosistion_en;
typedef enum
{
	MANUFACTURING_MODE_ON = 0,
	MANUFACTURING_MODE_OFF,
}manufacturingModeState_e;

typedef struct
{
	bool status_updated;
	bool sblcp_timeout_command;
	bool manufacturing_Mode;
	uint16_t timeout;
	uint16_t previous_received_command;
}sblcpStatusInfo_st;
//--------------------------------------------------------------------------------------------------
//                    External (Visible) Function Prototypes
//--------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------
//                    Internal Function Prototypes
//--------------------------------------------------------------------------------------------------

int8_t init_SBLCP();
//int8_t verify_hash(psCipherType_e type, const unsigned char *key, psSize_t keyLen,
//	    const unsigned char *buf, uint32_t msgbody_len,
//	    unsigned char hash[MAX_HASHLEN], unsigned char received_hash[MAX_HASHLEN], uint16_t in_message_code);
int8_t verify_hash(uint8_t *prx_buffer, uint16_t sblcp_msg_type, int length);
uint8_t init_sequence_number(void);
void updateSequenceNumber();
void get_serial_number(getSeqNumberMesg_st *replyMesg);
uint32_t get_protocol_ver();
void calculate_hash(uint8_t *pMsgData, uint16_t msgdata_size, uint8_t *replyMesg);
int16_t prepare_sblcp_response(uint8_t *prx_buffer, uint16_t in_message_code, sblcpResponce_st *pReply_Msg, uint8_t Ack_Nack);


void update_responce_header(sblcpResponce_st *header, uint8_t *rxBuff);
void getNextUDPSeqNoReply(uint8_t *reply_message, uint8_t *rxBuff);
uint8_t setNextUDPSeqNoReply();
void setRemoteHandlePosistion(uint8_t *rxBuff);
void getBreakerStatus(breakerStatus_st *reply_message, uint8_t *rxBuff);
uint8_t setLEDConfiguration(uint8_t *rxBuff);
size_t getMessageDataSize(uint16_t in_message_code);
int8_t getMessage(uint16_t in_message_code, uint8_t *prx_buffer, uint8_t *psblcp_msg, uint8_t reponse);
int8_t checkSequenceNumber(uint32_t rxSeqNumber, uint16_t rxMsgCode);
void setSequenceNumber(uint8_t *rxBuff, uint8_t *psblcp_msg);
void getMetroData(meterTelemetryData_st *metroData);
void Metro_startAvgMetroDataRead();
void Metro_setBreakerCalibration(uint8_t *rxBuff);
void sblcpSetSecondaryHandle(uint8_t handle_posistion);
void SBLCP_getSetAvgTeleData(meterTelemetryData_st *telemetry_data);
int16_t prepare_sblcp_timeout_response(uint8_t *prx_buffer, uint16_t in_message_code, sblcpResponce_st *pReply_Msg, uint8_t Ack_Nack);
#if SBLCP_DEBUG
void print_reposnce(void *pMsgForPrint, uint16_t msg_code);
#endif
#endif /* CODE_SBLCP_SBLCP_H_ */

