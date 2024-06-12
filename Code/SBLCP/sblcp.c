//--------------------------------------------------------------------------------------------------
//                      Eaton Electrical
//
//                      Proprietary Information
//                      (C) Copyright xxx, xxx
//                      All rights reserved
//
//--------------------------------------------------------------------------------------------------
//
//  Written by:         Rahul Sakhare
//                      Nitin Bhosale
//
//--------------------------------------------------------------------------------------------------
//  Product:            Smart Breaker Local communication Protocol
//
//  Module Name:        SBLCP.C
//                      Program module containing the following functions:
//                      - Protect_Init()     Init protection variables.  Called prior to starting
//                                           sampling & protection systems.
//
//                      - Sequence()         Called in main loop every line cycle to compute
//                                               DFTs & sequence components.
//
//                      - Reclose_Trip()     Called in main loop every line cycle after Sequence()
//                                               to perform trip & close protection algorithms.
//
//                      - Seq()              Called by Sequence() to compute the symmetrical
//                                               components from the fundamental frequency phasors
//                                               created by the DFT.
//
//                      - Mag()              Utility routine called by Sequence() to compute magnitude
//                                               of a complex number.
//
//                      - Output_SM()        Called by Reclose_Trip() to manage
//                                                trip / close / float outputs.
//
//--------------------------------------------------------------------------------------------------
//
//  Processor:          ESP32 PICO MCU
//
//  Target Hardware:
//
//  Compiler:           This file is compiled with "ESP-IDF v4.4.0" from Espressif
//
//--------------------------------------------------------------------------------------------------
//
//  Revision History:
//

//---------------------------------------------------------------------------------------------------------/

#include <string.h>
#include <stdlib.h>
#include <time.h>
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
#include "EventManager.h"
#include "Flash.h"
#include "Log.h"


//--------------------------------------------------------------------------------------------------
//                    Global Variable Definitions
//                          These variables are used by other modules
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//                    Local Variable Definitions
//                          These variables are only used in this module
//--------------------------------------------------------------------------------------------------

#define SBLCP_UDP_PORT 32866
#define CO_ORDINATOR_START_MESSAGE	0x45544e4d 	//"ETNM" //1163152973
#define REAPLY_START_MESSAGE		0x45544E53	//"ETNS" //1163152979
#define HEADER_LENGTH				10
#define SEQUENCE_NUMBER_LIMIT		100
#define FACTORY_MODE_DISABLE		0xA5

#define GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER	0x0000
#define GET_DEVICE_STATUS						0xFF00
#define GET_REMOTE_HANDLE_POSISTION				0x0001
#define GET_METER_TELEMETRY_DATA				0x0002
#define GET_PRIMARY_HANDLE_STATUS				0x0003
#define SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER	0x0080
#define SET_REMOTE_HANDLE_POSISTION				0x0081
#define SET_LED_TO_USER_DEFINED_COLOUR			0x0083
#define SET_BREAKER_EOL_CALIB_PARAM             0x0084
#define GET_BREAKER_EOL_CALIB_PARAM				0x0085
#define RESET_ENERGY							0x0086
#define SET_MANUFACTURING_RESET					0x0087
#define SET_MANUFACTURING_MODE_ON				0x0091
#define SET_MANUFACTURING_MODE_OFF				0x0092
#define UNKNOWN_COMMAND							0xFFFF
#define EPOCH_TIME_Jan_1_2023           1672549200
#define ONE_SEC							10
#define TWO_SEC							20

static const char *TAG = "example";

static uint32_t g_sequenceNumber;
static uint32_t g_getNextUDPSeqNumber;
sblcpStatusInfo_st replyStatusInfo;
static uint8_t g_SblcpFactoryMode;


uint8_t g_serialNo[] = {34,30,30,30,30,63,32,61,36,39,31,31,32,62,36,66};

//--------------------------------------------------------------------------------------------------
//                    External (Visible) Function Prototypes
//--------------------------------------------------------------------------------------------------

extern DRAM_ATTR Device_Info_str DeviceInfo;
//extern M2M_UART_COMMN gm2m_uart_comm;

//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        sblcp_task()
//--------------------------------------------------------------------------------------------------
/**
 * @brief   	This is task which handles the UDP packet reception and sending over socket.
 * 				Also this task initializes the Scoket port for data exchage over UDP.
 * 				Task is initialized when sb2.0 is boot-up.
 * @param[in] 	Taks parameters.
 * @retval     	None
*/
//--------------------------------------------------------------------------------------------------

static void sblcp_task(void *pvParameters)
{
    uint8_t rx_buffer[64];
    char udp_client_str[128];
    int socket_type = (int)pvParameters;
    int ip_protocol = 0;
    int max_soc_handle;
    struct sockaddr_in6 udp_server_addr;
//    uint8_t handle_status_timeout = 0;
    printf("SBLCP task init \n");


	uint32_t in_start_Of_mesg = 0;
	uint32_t in_seq_number    = 0;
	uint16_t in_message_code  =  0;
	uint16_t msg_data_size;
	uint8_t Ack_Nack_response = DONT_CARE;
//	uint16_t send_remote_handle_pos = 0;
	unsigned char received_hash[MAX_HASHLEN] = {0};
	uint8_t *pHashPtr = NULL;
	sblcpResponce_st *pReply = NULL;
	int16_t reply_length = 0;
	fd_set rfds, mfds;
	struct timeval tv;


	if (socket_type == AF_INET)
	{
		struct sockaddr_in *udp_server_addr_ip4 = (struct sockaddr_in *)&udp_server_addr;
		udp_server_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
		udp_server_addr_ip4->sin_family = AF_INET;
		udp_server_addr_ip4->sin_port = htons(SBLCP_UDP_PORT);
		ip_protocol = IPPROTO_IP;
		printf("Ip Address %d\n", udp_server_addr_ip4->sin_addr.s_addr);
	}
	else
	{
		if (socket_type == AF_INET6)
		{
			bzero(&udp_server_addr.sin6_addr.un, sizeof(udp_server_addr.sin6_addr.un));
			udp_server_addr.sin6_family = AF_INET6;
			udp_server_addr.sin6_port = htons(SBLCP_UDP_PORT);
			ip_protocol = IPPROTO_IPV6;
		}
	}

	int socket_handle = socket(socket_type, SOCK_DGRAM, ip_protocol);
	if (socket_handle < 0)
	{
		printf("Unable to create socket \n");
	}
	printf("Socket created \n");

	int err = bind(socket_handle, (struct sockaddr *)&udp_server_addr, sizeof(udp_server_addr));
	if (err < 0)
	{
		printf("Socket unable to bind\n");
	}
	printf("Socket bound, port %d \n", SBLCP_UDP_PORT);

	//Add a file descriptor to a file descriptor set.
	FD_ZERO(&rfds);
	max_soc_handle = socket_handle + 1;

#if defined(CONFIG_EXAMPLE_IPV4) && defined(CONFIG_EXAMPLE_IPV6)
        if (socket_type == AF_INET6) {
            // Note that by default IPV6 binds to both protocols, it is must be disabled
            // if both protocols used at the same time (used in CI)
            int opt = 1;
            setsockopt(socket_handle, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            setsockopt(socket_handle, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
        }
#endif

    while (1)
	{
		struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
		socklen_t socklen = sizeof(source_addr);
    	struct timeval to;
    	to.tv_sec = 0;
     	to.tv_usec = 100000;

		in_start_Of_mesg = 0;
		in_seq_number    = 0;
		in_message_code  = 0;

		if(DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_NOT_IN_A_NETWORK)
		{
			//device is un-provisioned SBLCP will not work as
			//Wi-Fi Credentials will be available delete the SBLCP task
			//SBLCP will re initialize again when credentials are available
			//exit from while loop to delete the task
			DeviceInfo.bSblcpInit = false;
			break;
		}

		FD_SET(socket_handle, &rfds);
		if (select(max_soc_handle, &rfds, NULL, NULL, &to) < 0)
		{
			printf("Select error\n");
			perror("select()");

		}
		else
		{
			if(FD_ISSET(socket_handle, &rfds))
			{
				memset(rx_buffer, 0x00, sizeof(rx_buffer));
				printf("################## Waiting for data #####################\n");
				int len = recvfrom(socket_handle, rx_buffer, (sizeof(rx_buffer) - 1), 0, (struct sockaddr *)&source_addr, &socklen);
				if (len < 0)
				{
					printf("recvfrom failed \n");
					continue;
				}
				// Data received
				else
				{
					// Get the sender's ip address as string
					if (source_addr.ss_family == PF_INET)
					{
						inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, udp_client_str, sizeof(udp_client_str) - 1);
					}
					else if (source_addr.ss_family == PF_INET6)
					{
						inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, udp_client_str, sizeof(udp_client_str) - 1);
					}

					rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
					printf("\nReceived %d bytes from %s:\n",len, udp_client_str);

					//extract the data from received packet from client
					in_start_Of_mesg = EXTRACT_DATA_4_BYTE(rx_buffer, 0);
					in_seq_number = EXTRACT_DATA_4_BYTE(rx_buffer, 4);
					in_message_code = EXTRACT_DATA_2_BYTE(rx_buffer, 8);
#if SBLCP_DEBUG
					printf("received start byte %x;\nreceived seq.number %x;\nreceived mesg code %x \n", in_start_Of_mesg, in_seq_number, in_message_code);
#endif
					if((in_message_code == GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER) &&
							(replyStatusInfo.previous_received_command == GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER) &&
							(replyStatusInfo.timeout < TWO_SEC))
					{
						//SBLCP should not respond to GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER command if it received
						//consecutive in less than two seconds. This time limit added to avoid
						//traffic and repetitive response to GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER on SBLCP network,
						//as GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER is broadcast command from SBLCP client to synchronize the network
						//may be broadcasted multiple times by client
#if SBLCP_DEBUG
						printf("GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER received in less than two second \n");
#endif
						continue;
					}
					else
					{
						replyStatusInfo.previous_received_command = in_message_code;
						replyStatusInfo.timeout = 0;
						if(in_start_Of_mesg == CO_ORDINATOR_START_MESSAGE)
						{

#if SBLCP_DEBUG
						printf("start of Message byte matches \n");
#endif
							//if(verify_hash(type, key, keyLen, buf,  msgbody_len, hash, received_hash, in_message_code))
							if(verify_hash(rx_buffer, in_message_code, len))
							{
#if SBLCP_DEBUG
								printf("Cryptographic hash check pass \n");
#endif
								if(checkSequenceNumber(in_seq_number, in_message_code) == SBLCP_SUCCESS)
								{
#if SBLCP_DEBUG
									printf("sequence number check pass \n");
#endif
									msg_data_size = getMessageDataSize(in_message_code);

									reply_length = (sizeof(sblcpResponce_st) + msg_data_size - sizeof(uint32_t));

									pReply = (sblcpResponce_st *)malloc(reply_length);
									if(pReply != NULL)
									{
										if(((prepare_sblcp_response(rx_buffer, in_message_code, pReply, Ack_Nack_response)) > 0) &&
												(pReply != NULL))
										{
											if(replyStatusInfo.sblcp_timeout_command == false)
											{
#if SBLCP_DEBUG
												print_reposnce(pReply, in_message_code);
#endif
												int err = sendto(socket_handle, (void *)pReply, reply_length, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
												if (err < 0)
												{
													printf("Error occurred during sending \n");
												}
											}
											free(pReply);
										}
									}

								}
								else
								{
									printf("sequence number check fails \n");
									updateSequenceNumber();
								}
							}
							else
							{
								printf("Cryptographic hash check fails \n");
								updateSequenceNumber();
							}
						}//message code check
						else
						{
							printf("Start of message byte dose not match \n");
							updateSequenceNumber();
						}
					}
				}//else
//				replyStatusInfo.timeout++;
			}
			else
			{
				//when response for command can not be updated or received immediate due to required operation is
				//handled in other module task based on received command.
				//SBLCP will with for the response to update for 1 seconds, if response is not received within one seconds
				//reply will be updated with negative acknowledge.
				if((replyStatusInfo.status_updated == true) || ((replyStatusInfo.sblcp_timeout_command == true) && (replyStatusInfo.timeout == TWO_SEC)))
				{
					//code comes here when response is updated for received command or SBLCP timeout is reached.
					uint8_t Ack_Nack;
					in_message_code = EXTRACT_DATA_2_BYTE(rx_buffer, 8);
					msg_data_size = getMessageDataSize(in_message_code);
					reply_length = (sizeof(sblcpResponce_st) + msg_data_size - sizeof(uint32_t));
					pReply = (sblcpResponce_st *)malloc(reply_length);
					if(pReply == NULL)
					{
						printf("@@@@@ 11 SBLCP_ERROR :: responce memory not allocated @@@@@\n");
					}

					if(replyStatusInfo.timeout == TWO_SEC)
					{
						//if code comes here means status is not updated, response is sent with negative acknowledge
#if SBLCP_DEBUG
						printf("Sending Negative ACK\n");
#endif
						replyStatusInfo.status_updated = true;
						Ack_Nack = NACK;
					}
					else
					{
						//if code comes here means status is updated, response is sent with positive acknowledge
						Ack_Nack = ACK;
					}

					if((prepare_sblcp_response(rx_buffer, in_message_code, pReply, Ack_Nack)) > 0)
					{
#if SBLCP_DEBUG
						print_reposnce(pReply, in_message_code);
#endif
						int err = sendto(socket_handle, (void *)pReply, reply_length, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
						if (err < 0)
						{
							printf("Error occurred during sending \n");
						}
						replyStatusInfo.timeout = 0;
						replyStatusInfo.sblcp_timeout_command = false;
						replyStatusInfo.status_updated = false;
					}
					free(pReply);

				}
				replyStatusInfo.timeout ++;
				continue;
			}//else FD_ISSET
		}// socket select else
	}//while(1)

	if (socket_handle != -1)
	{
		ESP_LOGE(TAG, "Shutting down socket and restarting...");
		shutdown(socket_handle, 0);
		close(socket_handle);
	}
	printf("Exiting the SBLCP Task\n");
    vTaskDelete(NULL);
}

//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION       init_SBLCP
//--------------------------------------------------------------------------------------------------
/**
 * @brief   	Function called after boot up of SB2.0
 * 				initializes the sequence number and SBLCP task.
 * @param[in] 	None
 * @retval     	SBLCP_SUCCESS or SBLCP_ERROR
*/
//--------------------------------------------------------------------------------------------------
int8_t init_SBLCP()
{
	int8_t err = SBLCP_SUCCESS;
	if(init_sequence_number() == SBLCP_SUCCESS)
	{
		xTaskCreate(sblcp_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
	}
	else
	{
		err = false;
	}

	g_SblcpFactoryMode = ReadByteFromFlash(EEOFFSET_FACTORY_MODE);

	if(g_SblcpFactoryMode != FACTORY_MODE_DISABLE)
	{
		replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_ON;
	}
	else
	{
		replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_OFF;
	}
	replyStatusInfo.status_updated = false;
	replyStatusInfo.sblcp_timeout_command = false;
	replyStatusInfo.timeout = 0;
	////at power on previous command is set to unknown
	replyStatusInfo.previous_received_command = UNKNOWN_COMMAND;
	return err;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        checkSequenceNumber()
//--------------------------------------------------------------------------------------------------
/**
 * @brief   	Function validates the sequence number. Validates the received and local sequence number.
 * 				also maintains the range of sequence number when required to set new sequence number
 *
 * @param[in] 	1. sequence number received from client; 2. message code
 * @retval     	SBLCP_SUCCESS or SBLCP_ERROR
*/
//--------------------------------------------------------------------------------------------------
int8_t checkSequenceNumber(uint32_t rxSeqNumber, uint16_t rxMsgCode)
{
	int8_t err = SBLCP_SUCCESS;
	if(rxMsgCode == GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER)
	{
		//for GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER command sequence
		//number received from client is always 0x0000
		if(rxSeqNumber == 0x0000)
		{
			err = SBLCP_SUCCESS;
		}
	}
	else
	{
		if(g_sequenceNumber == rxSeqNumber)
		{
			// sequence number should in the range as mentioned below
			// its should be greater than sequence number set on boot of device or sequence number set by client and
			// should be less that set sequence number + 100
			if((rxSeqNumber >= g_getNextUDPSeqNumber) && (rxSeqNumber <= (g_getNextUDPSeqNumber + SEQUENCE_NUMBER_LIMIT)))
			{
				err = SBLCP_SUCCESS;
			}
			else
			{
				printf("received Sequence number is out of range\n");
				err = SBLCP_ERROR;
			}
		}
		else
		{
			printf("received and current Sequence match fails\n");
			err = SBLCP_ERROR;
		}
	}
	return err;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        getMessage()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function calls the API to assemble the response
 * 				to be sent based on command received from client
 *
 * @param[in] 	1. message code; 2. data received from client; 3. pointer to response message
 * @retval     	SBLCP_SUCCESS or SBLCP_ERROR
*/
//--------------------------------------------------------------------------------------------------
int8_t getMessage(uint16_t in_message_code, uint8_t *prx_buffer, uint8_t *psblcp_msg, uint8_t response)
{
	int16_t iRet = SBLCP_SUCCESS;
	switch(in_message_code)
	{
		case GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER:
		{
#if SBLCP_DEBUG
			printf("preparing reply for GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER\n");
#endif
			getNextUDPSeqNoReply(psblcp_msg, prx_buffer);
			updateSequenceNumber();
		}
		break;
		case SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER:
		{
#if SBLCP_DEBUG
			printf("preparing reply for SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER\n");
#endif
			setSequenceNumber(prx_buffer, psblcp_msg);
			updateSequenceNumber();
		}
		break;
		case GET_DEVICE_STATUS:
		{
#if SBLCP_DEBUG
			printf("preparing reply for GET_DEVICE_STATUS\n");
#endif
			getBreakerStatus(psblcp_msg, prx_buffer);
			updateSequenceNumber();
		}
		break;
		case GET_METER_TELEMETRY_DATA:
		{
			if(replyStatusInfo.manufacturing_Mode == MANUFACTURING_MODE_ON)
			{
				//When manufacturing mode is on and request for telemetry is received
				//telemetry data will be average of 50 samples read from ADE9039
				//on every 20 msec interval
				if(!replyStatusInfo.status_updated)
				{
					Metro_startAvgMetroDataRead();
					replyStatusInfo.sblcp_timeout_command = true;
				}
				else
				{
#if SBLCP_DEBUG
			printf("preparing reply for GET_METER_TELEMETRT_DATA\n");
#endif
					SBLCP_getSetAvgTeleData(psblcp_msg);
					updateSequenceNumber();
				}

			}
			else
			{
#if SBLCP_DEBUG
			printf("preparing reply for GET_METER_TELEMETRT_DATA\n");
#endif
				getMetroData(psblcp_msg);
				updateSequenceNumber();
			}
		}
		break;
		case GET_REMOTE_HANDLE_POSISTION:
		{
#if SBLCP_DEBUG
			printf("preparing reply for GET_REMOTE_HANDLE_POSISTION\n");
#endif
			*psblcp_msg = DeviceInfo.SecondaryContactState;
			updateSequenceNumber();
		}
		break;
		case GET_PRIMARY_HANDLE_STATUS:
		{
#if SBLCP_DEBUG
			printf("preparing reply for GET_PRIMARY_HANDLE_STATUS\n");
#endif
			*psblcp_msg = DeviceInfo.PrimaryContactState;
			updateSequenceNumber();
		}
		break;
		case SET_REMOTE_HANDLE_POSISTION:
		{
#if SBLCP_DEBUG
			printf("preparing reply for SET_REMOTE_HANDLE_POSISTION\n");
#endif
			if(!replyStatusInfo.status_updated)
			{
				replyStatusInfo.sblcp_timeout_command = true;
				setRemoteHandlePosistion(prx_buffer);
			}
			else
			{
				setHandlePosition_st secondary_handle_status;
				secondary_handle_status.responce = response;
				secondary_handle_status.handle_posistion = DeviceInfo.SecondaryContactState;
				memcpy(psblcp_msg, &secondary_handle_status, sizeof(setHandlePosition_st));
				updateSequenceNumber();
			}
		}
		break;
		case SET_LED_TO_USER_DEFINED_COLOUR:
		{
#if SBLCP_DEBUG
			printf("preparing reply for SET_LED_TO_USER_DEFINED_COLOUR\n");
#endif
			*psblcp_msg = setLEDConfiguration(prx_buffer);
			updateSequenceNumber();
		}
		break;
		case SET_BREAKER_EOL_CALIB_PARAM:
		{
#if SBLCP_DEBUG
			printf("preparing reply for SET_BREAKER_EOL_CALIB_PARAM\n");
#endif
			if(!replyStatusInfo.status_updated)
			{
				replyStatusInfo.sblcp_timeout_command = true;
				BreakerEOLCalibration_st *calibData = malloc(sizeof(BreakerEOLCalibration_st));
				memcpy(calibData, prx_buffer + sizeof(sblcpHeader_st), sizeof(BreakerEOLCalibration_st));

				Metro_setBreakerCalibration(calibData);
				free(calibData);
			}
			else
			{
				memcpy(psblcp_msg, &response, sizeof(response));
				updateSequenceNumber();
			}

		}
		break;
		case GET_BREAKER_EOL_CALIB_PARAM:
		{
#if SBLCP_DEBUG
			printf("preparing reply for GET_BREAKER_EOL_CALIB_PARAM\n");
#endif
			BreakerEOLCalibration_st calib_factors;
			size_t length;

			length = ReadDataFromFlash(EEOFFSET_METRO_CALIB_LABEL, &calib_factors);
			if(length > 0)
			{
				memcpy(psblcp_msg, &calib_factors, sizeof(BreakerEOLCalibration_st));
			}
			else
			{
				memset(psblcp_msg, 0, sizeof(BreakerEOLCalibration_st));
			}
			updateSequenceNumber();
		}
		break;
		case SET_MANUFACTURING_MODE_ON:
		{
#if SBLCP_DEBUG
			printf("preparing reply for SET_MANUFACTURING_MODE_ON\n");
#endif
			if(g_SblcpFactoryMode != FACTORY_MODE_DISABLE)
			{
				*psblcp_msg = ACK;
				replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_ON;
			}
			else
			{
				*psblcp_msg = NACK;
				replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_OFF;
			}
			updateSequenceNumber();
		}
		break;
		case SET_MANUFACTURING_MODE_OFF:
		{
#if SBLCP_DEBUG
			printf("preparing reply for SET_MANUFACTURING_MODE_OFF\n");
#endif
			replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_OFF;
			*psblcp_msg = ACK;
			updateSequenceNumber();
		}
		break;
		case RESET_ENERGY:
		{
			int8_t err;
#if SBLCP_DEBUG
			printf("preparing reply for RESET_ENERGY\n");
#endif
			*psblcp_msg = ACK;

			err = reset_energy();
			if(err == SBLCP_ERROR)
			{
				*psblcp_msg = NACK;
			}
			else
			{
				if(replyStatusInfo.manufacturing_Mode == MANUFACTURING_MODE_ON)
				{
					err = reset_nonResettable_energy();
					if(err == SBLCP_ERROR)
					{
						*psblcp_msg = NACK;
					}
				}
			}
			updateSequenceNumber();
		}
		break;
		case SET_MANUFACTURING_RESET:
		{
#if SBLCP_DEBUG
			printf("preparing reply for SET_MANUFACTURING_RESET\n");
#endif
			*psblcp_msg = NACK;
			WriteByteToFlash(EEOFFSET_FACTORY_MODE, FACTORY_MODE_DISABLE);
			ResetDevice(MANUFACTURING_RESET, true);
			updateSequenceNumber();
		}
		break;
		default:
			break;
	}
	return iRet;//add valid error returns
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        getMessageDataSize()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function calculates the size of response message body
 *
 * @param[in] 	1. message code;
 * @retval     	size of the message body
*/
//--------------------------------------------------------------------------------------------------
size_t getMessageDataSize(uint16_t in_message_code)
{
	size_t msg_size = 0;
	if(in_message_code ==  GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER)
	{
		msg_size = sizeof(getSeqNumberMesg_st);
	}
	else if(in_message_code ==  GET_DEVICE_STATUS)
	{
		msg_size = sizeof(breakerStatus_st);
	}
	else if(in_message_code ==  GET_REMOTE_HANDLE_POSISTION)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code ==  SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code ==  SET_REMOTE_HANDLE_POSISTION)
	{
		msg_size = sizeof(setHandlePosition_st);
	}
	else if(in_message_code ==  SET_LED_TO_USER_DEFINED_COLOUR)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code == SET_MANUFACTURING_MODE_ON)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code == SET_MANUFACTURING_MODE_OFF)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code == GET_BREAKER_EOL_CALIB_PARAM)
	{
		msg_size = sizeof(BreakerEOLCalibration_st);
	}
	else if(in_message_code ==  SET_BREAKER_EOL_CALIB_PARAM)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code == GET_METER_TELEMETRY_DATA)
	{
		msg_size = sizeof(meterTelemetryData_st);
	}
	else if(in_message_code ==  RESET_ENERGY)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code ==  GET_PRIMARY_HANDLE_STATUS)
	{
		msg_size = sizeof(uint8_t);
	}
	else
	{
		if(in_message_code ==  SET_MANUFACTURING_RESET)
		{
			msg_size = sizeof(uint8_t);
		}
	}
	return msg_size;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        prepare_sblcp_response()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates the complete response for received command.
 * 				Updates the response header as well as hash.
 *
 * @param[in] 	1.received data from client; 2.message code; 3. pointer to response message
 * @retval     	SBLCP_SUCCESS or SBLCP_ERROR
*/
//--------------------------------------------------------------------------------------------------
//int16_t prepare_sblcp_response(uint8_t *prx_buffer, uint16_t in_message_code, sblcpResponce_st *pReply_Msg)
//{
//	int16_t iRet = SBLCP_SUCCESS;
//	sblcpResponce_st heade_footer;
//	uint8_t *pMsgPtr = NULL;
//	uint8_t *pHashPtr = NULL;
//	uint8_t* pReply = (uint8_t *)pReply_Msg;
//	uint16_t msg_data_size = getMessageDataSize(in_message_code);
//	uint32_t msgdata_len = 0;
//	msgdata_len = (sizeof(sblcpHeader_st) + msg_data_size);
//
//	if(iRet > SBLCP_ERROR)
//	{
//		if((replyStatusInfo.manufacturing_Mode == MANUFACTURING_MODE_OFF) &&
//				((in_message_code == SET_BREAKER_EOL_CALIB_PARAM) || (in_message_code == GET_BREAKER_EOL_CALIB_PARAM)))
//		{
//			//metro calibration should be done only if manufacturing mode is on
//			iRet = SBLCP_ERROR;
//		}
//		else
//		{
//			//get the pointer for response body
//			pMsgPtr = pReply + sizeof(pReply_Msg->sblcp_header);
//			if(pMsgPtr == NULL)
//			{
//				printf("@@@@@ 11 SBLCP_ERROR :: msg memory not allocated @@@@@\n");
//				iRet = SBLCP_ERROR;
//			}
//			// get the pointer for crypto has
//			pHashPtr = (uint8_t *)pReply + msg_data_size + sizeof(pReply_Msg->sblcp_header);
//			if(pHashPtr == NULL)
//			{
//				printf("@@@@@ 11 SBLCP_ERROR :: msg memory not allocated @@@@@\n");
//				iRet = SBLCP_ERROR;
//			}
//			if(iRet > SBLCP_ERROR)
//			{
//				uint8_t message_body[512] = {0};//array size needs to be limited
//				//update the reponce header
//				update_responce_header(&heade_footer.sblcp_header, prx_buffer);
//				//get the response into response body
//				getMessage(in_message_code, prx_buffer, message_body);
//				printf("\n");
//
//				memcpy(pReply, (uint8_t *)&heade_footer.sblcp_header, sizeof(heade_footer.sblcp_header));
//
//				memcpy(pMsgPtr, message_body, msg_data_size);
//				//calculate the crypto hash on header and message body
//				calculate_hash(pReply, msgdata_len, heade_footer.sblcp_hash);
//				memcpy(pHashPtr, heade_footer.sblcp_hash, sizeof(heade_footer.sblcp_hash));
//			}
//		}
//	}
//	return iRet;
//}
int16_t prepare_sblcp_response(uint8_t *prx_buffer, uint16_t in_message_code, sblcpResponce_st *pReply_Msg, uint8_t response)
{
	int16_t iRet = SBLCP_SUCCESS;
	sblcpResponce_st heade_footer;
	uint8_t *pMsgPtr = NULL;
	uint8_t *pHashPtr = NULL;
	uint8_t* pReply = (uint8_t *)pReply_Msg;
	uint16_t msg_data_size = getMessageDataSize(in_message_code);
	uint32_t msgdata_len = 0;
	msgdata_len = (sizeof(sblcpHeader_st) + msg_data_size);

	if(iRet > SBLCP_ERROR)
	{
		if((replyStatusInfo.manufacturing_Mode == MANUFACTURING_MODE_OFF) &&
				((in_message_code == SET_BREAKER_EOL_CALIB_PARAM) || (in_message_code == GET_BREAKER_EOL_CALIB_PARAM)))
		{
			//metro calibration should be done only if manufacturing mode is on
			iRet = SBLCP_ERROR;
		}
		else
		{
			//get the pointer for response body
			pMsgPtr = pReply + sizeof(pReply_Msg->sblcp_header);
			if(pMsgPtr == NULL)
			{
				printf("@@@@@ 11 SBLCP_ERROR :: msg memory not allocated @@@@@\n");
				iRet = SBLCP_ERROR;
			}
			// get the pointer for crypto has
			pHashPtr = (uint8_t *)pReply + msg_data_size + sizeof(pReply_Msg->sblcp_header);
			if(pHashPtr == NULL)
			{
				printf("@@@@@ 11 SBLCP_ERROR :: msg memory not allocated @@@@@\n");
				iRet = SBLCP_ERROR;
			}
			if(iRet > SBLCP_ERROR)
			{
				uint8_t message_body[512] = {0};//array size needs to be limited
				//update the reponce header
				update_responce_header(&heade_footer.sblcp_header, prx_buffer);
				//get the response into response body
				getMessage(in_message_code, prx_buffer, message_body, response);
				printf("\n");

				memcpy(pReply, (uint8_t *)&heade_footer.sblcp_header, sizeof(heade_footer.sblcp_header));

				memcpy(pMsgPtr, message_body, msg_data_size);
				//calculate the crypto hash on header and message body
				calculate_hash(pReply, msgdata_len, heade_footer.sblcp_hash);
				memcpy(pHashPtr, heade_footer.sblcp_hash, sizeof(heade_footer.sblcp_hash));
			}
		}
	}
	return iRet;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        verify_hash()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function validates the message received from client by validation the
 * 				crpto hash. Hash is calculated on the message headd and message boy using 32byte key
 *
 * @param[in] 	1.received data from client; 2.message code;
 * @retval     	SBLCP_SUCCESS or SBLCP_ERROR
*/
//--------------------------------------------------------------------------------------------------
//int8_t verify_hash(psCipherType_e type, const unsigned char *key, psSize_t keyLen,
//	    const unsigned char *buf, uint32_t msgbody_len,
//	    unsigned char hash[MAX_HASHLEN], unsigned char received_hash[MAX_HASHLEN], uint16_t in_message_code)
int8_t verify_hash(uint8_t *prx_buffer, uint16_t sblcp_msg_type, int length)
{

	int8_t iRet = SBLCP_SUCCESS;
	uint8_t *pHashPtr = NULL;
	uint16_t msg_data_size = getMessageDataSize(sblcp_msg_type);
	//CRYPTOGRAPHIC_HMAC_HASH crypto_hash_msg;
//	psCipherType_       etype = HMAC_SHA256;
	const unsigned char udp_key[CRYPTO_HASH_LENGTH] = "ba32b432c6e911edafa10242ac120002";
	psSize_t            udp_keyLen = 32;
	unsigned char       message_data[255];//need to use #define
	uint32_t            msgdata_len = 0;
	unsigned char       cryptop_hmac_hash[MAX_HASHLEN] = {0};

	//length of the message is calculated on which
	//Hash must calculated for verification
	msg_data_size = length - CRYPTO_HASH_LENGTH;
	memcpy(message_data, prx_buffer, msg_data_size);


//	printf("\nMSG BDY::11");
//	for(uint8_t i = 0; i < msgbody_len; i++)
//	{
//		printf("%x,",buf[i]);
//	}
//	printf("\n");
	pHashPtr = prx_buffer + msgdata_len;
	if(pHashPtr == NULL)
	{
//		printf("hash memory not allocated\n");
		//return error path
		iRet = SBLCP_ERROR;
	}
	else
	{
//		printf("\nMSG BDY::22\n");
		//add call to getudpkey()
		//crypto_hash_msg.udp_key = getudpkey();
		if ((iRet = psHmac(HMAC_SHA256, udp_key, udp_keyLen, message_data,  msgdata_len, cryptop_hmac_hash)) == 0)
		{
//			printf("HMAC_SHA256 DONE::\n");
			//do common return path
//			for(uint8_t i = 0; i < MAX_HASHLEN; i++)
//			{
//				printf("%x,",cryptop_hmac_hash[i]);
//			}
//			printf("\n");
			if(memcmp(pHashPtr, cryptop_hmac_hash, MAX_HASHLEN) == 0)
			{
				iRet = SBLCP_ERROR;
			}
		}
	}
	return SBLCP_SUCCESS;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        init_sequence_number()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function is called on initialization on boot up of SB2.0
 * 				It will initializes the sequence number to random number on every boot
 *
 * @param[in] 	1.received data from client; 2.message code;
 * @retval     	SBLCP_SUCCESS or SBLCP_ERROR
*/
//--------------------------------------------------------------------------------------------------
uint8_t init_sequence_number(void)
{
	uint32_t seconds;
	uint8_t err = SBLCP_SUCCESS;
	//to generate random number for initialization of sequence number
	// uses EPOC time as seed value for srand(function).
	//if seed is not used, every time random number generated will same number.
	seconds = time(0);
	if((seconds > EPOCH_TIME_Jan_1_2023) && (DeviceInfo.WiFiConnected))
	{
		srand(seconds);
		g_sequenceNumber = rand();
		g_getNextUDPSeqNumber = g_sequenceNumber;
		printf("\n***************init sequense_number 0x%x \n", g_sequenceNumber);
	}
	else
	{
		err = SBLCP_ERROR;
	}
	return err;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        get_serial_number()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function copies the device serial number
 *
 * @param[in] 	1. pointer to response message;
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void get_serial_number(getSeqNumberMesg_st *replyMesg)
{
	//TBD: Device serial number is not knows for now, once its available its will be replaced with actual serial number
	memcpy(replyMesg->serial_number, g_serialNo, sizeof(g_serialNo));
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        get_protocol_ver()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function returns the protocol version
 *
 * @param[in] 	none
 * @retval     	protocol version
*/
//--------------------------------------------------------------------------------------------------
uint32_t get_protocol_ver()
{
	uint32_t ver = 0x100000;
	return ver;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        calculate_hash()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function calculates crpto has on response.
 * 				Has is calculated on response header and message body using security key.
 *
 * @param[in] 	1. pointer to response header; 2. size of response message body; 3.pointer to reply
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void calculate_hash(uint8_t *pMsgData, uint16_t msgdata_size, uint8_t *replyMesg)
{
	const unsigned char udp_key[CRYPTO_HASH_LENGTH] = "ba32b432c6e911edafa10242ac120002";
	psSize_t            udp_keyLen = 32;
	unsigned char       cryptop_hmac_hash[MAX_HASHLEN] = {0};

	if ((psHmac(HMAC_SHA256, udp_key, udp_keyLen, pMsgData,  msgdata_size, replyMesg)) == 0)
	{
//		printf("HMAC_SHA256 DONE::\n");
		//do common return path
//		for(uint8_t i = 0; i < MAX_HASHLEN; i++)
//		{
//			printf("%x,",replyMesg[i]);
//		}
	}
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        getNextUDPSeqNoReply()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates the response for GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER command
 *
 * @param[in] 	1. pointer to response message; 2. received message
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void getNextUDPSeqNoReply(uint8_t *reply_message, uint8_t *rxBuff)
{
	getSeqNumberMesg_st *preply = NULL;


	preply = (getSeqNumberMesg_st *)reply_message;
	preply->seq_number = g_sequenceNumber;
	g_getNextUDPSeqNumber = g_sequenceNumber;


	get_serial_number(preply);
	preply->version = get_protocol_ver();
	preply->nonce = EXTRACT_DATA_4_BYTE(rxBuff, 10);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        update_responce_header()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates the header for response message.
 *
 * @param[in] 	1. pointer to response header; 2. received message
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void update_responce_header(sblcpResponce_st *header, uint8_t *rxBuff)
{
	header->sblcp_header.start_byte = REAPLY_START_MESSAGE;
	header->sblcp_header.mesg_code = EXTRACT_DATA_2_BYTE(rxBuff, 8);
	if(header->sblcp_header.mesg_code == GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER)
	{
		header->sblcp_header.seq_number = EXTRACT_DATA_4_BYTE(rxBuff, 4);
	}
	else
	{
		header->sblcp_header.seq_number = g_sequenceNumber;
	}
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        setRemoteHandlePosistion()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function sets the seconder handle to open or close based on
 * 				command received from client
 *
 * @param[in] 	1.received message
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void setRemoteHandlePosistion(uint8_t *rxBuff)
{
	uint8_t handle_posistion;
	uint8_t err = ACK;
	handle_posistion = EXTRACT_DATA_1_BYTE(rxBuff, 10);

	printf("Requested handle position %d\n", handle_posistion);
	sblcpSetSecondaryHandle(handle_posistion);

//	if(handle_posistion == HANDLE_OPEN)
//	{
//		//DeviceInfo.SecondaryContactState = LOAD_OFF;
//	    memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
//		//enable code and test on P0
//		prepare_uart_command(&gm2m_uart_comm, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_OPEN, NO_PAYLOAD_2, NO_PAYLOAD_2);
//	}
//	else if(handle_posistion == HANDLE_CLOSE)
//	{
//		//DeviceInfo.SecondaryContactState = LOAD_ON;
//	    memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
//		//enable code and test on P0
//		prepare_uart_command(&gm2m_uart_comm, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_CLOSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
//	}
//	else if(handle_posistion == TOGGLE)
//	{
//		if(DeviceInfo.SecondaryContactState == LOAD_OFF)
//		{
//			//DeviceInfo.SecondaryContactState = LOAD_ON;
//		    memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
//			//enable code and test on P0
//			prepare_uart_command(&gm2m_uart_comm, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_CLOSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
//		}
//		else
//		{
//			//DeviceInfo.SecondaryContactState = LOAD_OFF;
//		    memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
//			//enable code and test on P0
//			prepare_uart_command(&gm2m_uart_comm, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_OPEN, NO_PAYLOAD_2, NO_PAYLOAD_2);
//		}
//	}
//	else
//	{
//		err = NACK;
//	}
	return;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        setRemoteHandlePosistion()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates the response message for GET_DEVICE_STATUS
 * 				command received from client
 *
 * @param[in] 	1.pointer to response message; 2. received message
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void getBreakerStatus(breakerStatus_st *reply_message, uint8_t *rxBuff)
{
	//gets the meteorology data
	meterTelemetryData_st *metro_temetry_data;
	metro_temetry_data = &reply_message->telemetry_data;
	getMetroData(metro_temetry_data);

	//updates the secondary handle status
	reply_message->breaker_state = DeviceInfo.SecondaryContactState;
	reply_message->primary_handle_status = DeviceInfo.PrimaryContactState;
	reply_message->path_status = DeviceInfo.PathStatus;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        setLEDConfiguration()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function sets the the LED settings based on received configuration
 *
 * @param[in] 	1.received message
 * @retval     	ACK or NACK
*/
//--------------------------------------------------------------------------------------------------
uint8_t setLEDConfiguration(uint8_t *rxBuff)
{
	uint8_t *rx_led_states;
	setLedColour_st *led_states;

	rx_led_states = (uint8_t*)malloc(sizeof(setLedColour_st));
	if(rx_led_states == NULL)
	{
		return NACK;
	}
	memcpy(rx_led_states, rxBuff + sizeof(sblcpHeader_st) ,sizeof(setLedColour_st));

	led_states = (setLedColour_st*)rx_led_states;

	printf("led_colour %x\n",led_states->led_colour);
	printf("blinking_time_interval %x\n",led_states->blinking_time_interval);
	printf("blinking_time_expire %x\n",led_states->blinking_time_expire);
	printf("start_blinking_leds_flag %x\n",led_states->start_blinking_leds_flag);

//	add RGB color = payload 1, blinking_time_interval - payload 2, blinking_time_expire - payload 3
//	prepare_uart_command(&gm2m_uart_comm, LED_COLOR_BLINK_RATE, RGB_BLUE, NO_PAYLOAD_2, NO_PAYLOAD_2);
	free(rx_led_states);
	return ACK;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        updateSequenceNumber()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates the sequence number for next message.
 * 				sequence number must be incremented by one on every command.
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void updateSequenceNumber()
{
	if((g_sequenceNumber >= g_getNextUDPSeqNumber) && (g_sequenceNumber <= (g_getNextUDPSeqNumber + SEQUENCE_NUMBER_LIMIT)))
	{
		g_sequenceNumber = g_sequenceNumber + 1;
	}
	else
	{
#if SBLCP_DEBUG
		printf("Sequence number exceeds the range \n");
#endif
	}
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        setSequenceNumber()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function sets the sequence number to received sequence number
 * 				when SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER command is received
 *
 * @param[in] 	1.received message 2. pointer to responce message.
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void setSequenceNumber(uint8_t *rxBuff, uint8_t *psblcp_msg)
{
	uint32_t new_sequense_number;
	new_sequense_number = EXTRACT_DATA_4_BYTE(rxBuff, 10);
	if(new_sequense_number > (g_getNextUDPSeqNumber + SEQUENCE_NUMBER_LIMIT))
	{
		g_getNextUDPSeqNumber = new_sequense_number;
		g_sequenceNumber = new_sequense_number;
		*psblcp_msg = ACK;
	}
	else
	{
		*psblcp_msg = BAD_RESPONCE;
	}
//	g_sequenceNumber = EXTRACT_DATA_4_BYTE(rxBuff, 10);
//	g_getNextUDPSeqNumber = g_sequenceNumber;
//	*psblcp_msg = ACK;
}
////--------------------------------------------------------------------------------------------------
////            START OF FUNCTION        prepare_sblcp_timeout_response()
////--------------------------------------------------------------------------------------------------
///**
// * @brief		Function prepares the reply for commands whose response dose not updated immediate
// * 				 or status is not received within one second timeout
// *
// * @param[in] 	1.received message  2. message code 3. pointer to response message; 4.response (ACK or NACK)
// * @retval     	none
//*/
////--------------------------------------------------------------------------------------------------
//int16_t prepare_sblcp_timeout_response(uint8_t *prx_buffer, uint16_t in_message_code, sblcpResponce_st *pReply_Msg, uint8_t Ack_Nack)
//{
//	int16_t iRet = SBLCP_SUCCESS;
//	sblcpResponce_st heade_footer;
//	uint8_t *pMsgPtr = NULL;
//	uint8_t *pHashPtr = NULL;
//	uint8_t* pReply = (uint8_t *)pReply_Msg;
//	uint16_t msg_data_size = getMessageDataSize(in_message_code);
//	uint32_t msgdata_len = 0;
//	msgdata_len = (sizeof(sblcpHeader_st) + msg_data_size);
//
//	if(iRet > SBLCP_ERROR)
//	{
//		//get the pointer for response body
//		pMsgPtr = pReply + sizeof(pReply_Msg->sblcp_header);
//		if(pMsgPtr == NULL)
//		{
//			printf("@@@@@ 11 SBLCP_ERROR :: msg memory not allocated @@@@@\n");
//			iRet = SBLCP_ERROR;
//		}
//		// get the pointer for crypto has
//		pHashPtr = (uint8_t *)pReply + msg_data_size + sizeof(pReply_Msg->sblcp_header);
//		if(pHashPtr == NULL)
//		{
//			printf("@@@@@ 11 SBLCP_ERROR :: msg memory not allocated @@@@@\n");
//			iRet = SBLCP_ERROR;
//		}
//		if(iRet > SBLCP_ERROR)
//		{
//			uint8_t message_body[512] = {0};//array size needs to be limited
//			//update the response header
//			update_responce_header(&heade_footer.sblcp_header, prx_buffer);
//			printf("in_message_code %x\n", in_message_code);
//
//			getMessage(in_message_code, prx_buffer, pReply_Msg);
//
//			if(in_message_code == SET_REMOTE_HANDLE_POSISTION)
//			{
//				memcpy(pMsgPtr, &Ack_Nack, msg_data_size);
//			}
//			else if(in_message_code == SET_BREAKER_EOL_CALIB_PARAM)
//			{
//				memcpy(pMsgPtr, &Ack_Nack, msg_data_size);
//
//			}
//
//			memcpy(pReply, (uint8_t *)&heade_footer.sblcp_header, sizeof(heade_footer.sblcp_header));
//
//			//calculate the crypto hash on header and message body
//			calculate_hash(pReply, msgdata_len, heade_footer.sblcp_hash);
//			memcpy(pHashPtr, heade_footer.sblcp_hash, sizeof(heade_footer.sblcp_hash));
//		}
//	}
//	return iRet;
//}
void SBLCP_getSetAvgTeleData(meterTelemetryData_st *telemetry_data)
{
	static meterTelemetryData_st *Avg_data;

	if(Avg_data == NULL)
	{
		Avg_data = (meterTelemetryData_st*)malloc(sizeof(meterTelemetryData_st));
		memset(Avg_data, 0, sizeof(meterTelemetryData_st));
		if(replyStatusInfo.status_updated)
		{
			//Code comes here if 2 sec timeout is hit, so no telemetry data has been updated
			//response must be updated with 0 values for all telemetry parameters.
			memcpy(telemetry_data, Avg_data, sizeof(meterTelemetryData_st));
			free(Avg_data);
			Avg_data = NULL;
#if SBLCP_DEBUG
			printf("Timeout is hit, Telemetry data updated with zero\n");
#endif
		}
		else
		{
			memcpy(Avg_data, telemetry_data, sizeof(meterTelemetryData_st));
			replyStatusInfo.status_updated = true;
		}
	}
	else
	{
		memcpy(telemetry_data, Avg_data, sizeof(meterTelemetryData_st));
#if SBLCP_DEBUG
		printf("update metro for sending reply\n");
#endif
		free(Avg_data);
		Avg_data = NULL;
	}

}
#if SBLCP_DEBUG
void print_reposnce(void *pMsgForPrint, uint16_t msg_code)
{
	sblcpResponce_st *pRsponce = NULL;
	uint8_t *msg = NULL;

	msg = (uint8_t *)pMsgForPrint;
	pRsponce = (sblcpResponce_st *)pMsgForPrint;

	printf("********Header*********\n");
	printf("start_of_message %x\n", pRsponce->sblcp_header.start_byte);
	printf("sequence_number %x\n", pRsponce->sblcp_header.seq_number);
	printf("message_code %x\n", pRsponce->sblcp_header.mesg_code);
	printf("\n");

	if(msg_code == GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER)
	{
		printf("********Message Body*********\n");
		getSeqNumberMesg_st *pMsg_body = &pRsponce->message_data;

		printf("seq_number %x\n", pMsg_body->seq_number);
		for(uint8_t i = 0; i < 16; i++)
		{
			printf("%x,", pMsg_body->serial_number[i]);
		}
		printf("\n");
		printf("version %x\n", pMsg_body->version);
		printf("nonce %x\n", pMsg_body->nonce);
		printf("\n");
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(getSeqNumberMesg_st) + i]);
		}
		printf("]\n");
	}
	if(msg_code == GET_DEVICE_STATUS)
	{

		printf("********Message Body*********\n");
		breakerStatus_st *pMsg_body = &pRsponce->message_data;

//		printf("handle status %d\n", pMsg_body->breaker_state);
//		printf("Primary handle status %d\n", pMsg_body->primary_handle_status);
//		printf("line_frequency %f\n", pMsg_body->line_frequency);
//
//		printf("Phase_A_active_energy %f\n", pMsg_body->phase_A_active_energy);
//		printf("Phase_A_app_energy %f\n", pMsg_body->phase_A_app_energy);
//		printf("Phase_A_reactive_energy %f\n", pMsg_body->phase_A_reactive_energy);
//		printf("Phase_A_rms_current %f\n", pMsg_body->phase_A_rms_current);
//		printf("Phase_A_rms_voltage %f\n", pMsg_body->phase_A_rms_voltage);
//
//		printf("Phase_B_active_energy %f\n", pMsg_body->phase_B_active_energy);
//		printf("Phase_B_app_energy %f\n", pMsg_body->phase_B_app_energy);
//		printf("Phase_B_reactive_energy %f\n", pMsg_body->phase_B_reactive_energy);
//		printf("Phase_B_rms_current %f\n", pMsg_body->phase_B_rms_current);
//		printf("Phase_B_rms_voltage %f\n", pMsg_body->phase_B_rms_voltage);
		printf("handle status %d\n", pMsg_body->breaker_state);
		printf("Primary handle status %d\n", pMsg_body->primary_handle_status);

		printf("phase_A_line_frequency %f\n", pMsg_body->telemetry_data.phase_A_line_frequency);
		printf("Phase_A_rms_voltage %f\n", pMsg_body->telemetry_data.phase_A_rms_voltage);
		printf("Phase_A_rms_current %f\n", pMsg_body->telemetry_data.phase_A_rms_current);
		printf("phase_A_power_factor %f\n", pMsg_body->telemetry_data.phase_A_power_factor);
		printf("Phase_A_active_power %f\n", pMsg_body->telemetry_data.phase_A_active_power);
		printf("Phase_A_reactive_power %f\n", pMsg_body->telemetry_data.phase_A_reactive_power);
		printf("Phase_A_app_power %f\n", pMsg_body->telemetry_data.phase_A_app_power);
		printf("Phase_A_active_energy %f\n", pMsg_body->telemetry_data.phase_A_active_energy);
		printf("Phase_A_reactive_energy %f\n", pMsg_body->telemetry_data.phase_A_reactive_energy);
		printf("Phase_A_app_energy %f\n", pMsg_body->telemetry_data.phase_A_app_energy);
		printf("Phase_A_reverse_active_energy %f\n", pMsg_body->telemetry_data.phase_A_reverse_active_energy);
		printf("Phase_A_reverse_reactive_energy %f\n", pMsg_body->telemetry_data.phase_A_reverse_reactive_energy);
		printf("Phase_A_reverse_app_energy %f\n", pMsg_body->telemetry_data.phase_A_reverse_app_energy);
		printf("\n");
		printf("phase_B_line_frequency %f\n", pMsg_body->telemetry_data.phase_B_line_frequency);
		printf("Phase_B_rms_voltage %f\n", pMsg_body->telemetry_data.phase_B_rms_voltage);
		printf("Phase_B_rms_current %f\n", pMsg_body->telemetry_data.phase_B_rms_current);
		printf("phase_B_power_factor %f\n", pMsg_body->telemetry_data.phase_B_power_factor);
		printf("Phase_B_active_power %f\n", pMsg_body->telemetry_data.phase_B_active_power);
		printf("Phase_B_reactive_power %f\n", pMsg_body->telemetry_data.phase_B_reactive_power);
		printf("Phase_B_app_power %f\n", pMsg_body->telemetry_data.phase_B_app_power);
		printf("Phase_B_active_energy %f\n", pMsg_body->telemetry_data.phase_B_active_energy);
		printf("Phase_B_reactive_energy %f\n", pMsg_body->telemetry_data.phase_B_reactive_energy);
		printf("Phase_B_app_energy %f\n", pMsg_body->telemetry_data.phase_B_app_energy);
		printf("Phase_B_reverse_active_energy %f\n", pMsg_body->telemetry_data.phase_B_reverse_active_energy);
		printf("Phase_B_reverse_reactive_energy %f\n", pMsg_body->telemetry_data.phase_B_reverse_reactive_energy);
		printf("Phase_B_reverse_app_energy %f\n", pMsg_body->telemetry_data.phase_B_reverse_app_energy);



		printf("\n");
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(breakerStatus_st) + i]);
		}
		printf("]\n");
//		printf("meter_update_no %d\n", pMsg_body->meter_update_no);
//		printf("period %d\n", pMsg_body->period);
//		printf("ph0_Q1_act_energy %lld\n", pMsg_body->ph0_Q1_act_energy);
//		printf("ph0_Q1_app_energy %lld\n", pMsg_body->ph0_Q1_app_energy);
//		printf("ph0_Q1_reactive_energy %lld\n", pMsg_body->ph0_Q1_reactive_energy);
//		printf("ph0_Q2_act_energy %lld\n", pMsg_body->ph0_Q2_act_energy);
//		printf("ph0_Q2_app_energy %lld\n", pMsg_body->ph0_Q2_app_energy);
//		printf("ph0_Q2_reactive_energy %lld\n", pMsg_body->ph0_Q2_reactive_energy);
//		printf("ph0_Q3_act_energy %lld\n", pMsg_body->ph0_Q3_act_energy);
//		printf("ph0_Q3_app_energy %lld\n", pMsg_body->ph0_Q3_app_energy);
//		printf("ph0_Q3_reactive_energy %lld\n", pMsg_body->ph0_Q3_reactive_energy);
//		printf("ph0_Q4_act_energy %lld\n", pMsg_body->ph0_Q4_act_energy);
//		printf("ph0_Q4_app_energy %lld\n", pMsg_body->ph0_Q4_app_energy);
//		printf("ph0_Q4_reactive_energy %lld\n", pMsg_body->ph0_Q4_reactive_energy);
//		printf("ph0_Q5_act_energy %lld\n", pMsg_body->ph0_Q5_act_energy);
//		printf("ph0_Q5_app_energy %lld\n", pMsg_body->ph0_Q5_app_energy);
//		printf("ph0_Q5_reactive_energy %lld\n", pMsg_body->ph0_Q5_reactive_energy);
//		printf("ph1_Q1_act_energy %lld\n", pMsg_body->ph1_Q1_act_energy);
//		printf("ph1_Q1_app_energy %lld\n", pMsg_body->ph1_Q1_app_energy);
//		printf("ph1_Q1_reactive_energy %lld\n", pMsg_body->ph1_Q1_reactive_energy);
//		printf("ph1_Q2_act_energy %lld\n", pMsg_body->ph1_Q2_act_energy);
//		printf("ph1_Q2_app_energy %lld\n", pMsg_body->ph1_Q2_app_energy);
//		printf("ph1_Q2_reactive_energy %lld\n", pMsg_body->ph1_Q2_reactive_energy);
//		printf("ph1_Q3_act_energy %lld\n", pMsg_body->ph1_Q3_act_energy);
//		printf("ph1_Q3_app_energy %lld\n", pMsg_body->ph1_Q3_app_energy);
//		printf("ph1_Q3_reactive_energy %lld\n", pMsg_body->ph1_Q3_reactive_energy);
//		printf("ph1_Q4_act_energy %lld\n", pMsg_body->ph1_Q4_act_energy);
//		printf("ph1_Q4_app_energy %lld\n", pMsg_body->ph1_Q4_app_energy);
//		printf("ph1_Q4_reactive_energy %lld\n", pMsg_body->ph1_Q4_reactive_energy);
//		printf("ph1_Q5_act_energy %lld\n", pMsg_body->ph1_Q5_act_energy);
//		printf("ph1_Q5_app_energy %lld\n", pMsg_body->ph1_Q5_app_energy);
//		printf("ph1_Q5_reactive_energy %lld\n", pMsg_body->ph1_Q5_reactive_energy);
		printf("\n");
	}
	if(msg_code == GET_METER_TELEMETRY_DATA)
	{
		printf("********Message Body*********\n");
		meterTelemetryData_st *pMsg_body = &pRsponce->message_data;

		printf("phase_A_line_frequency %f\n", pMsg_body->phase_A_line_frequency);
		printf("phase_A_rms_voltage %f\n", pMsg_body->phase_A_rms_voltage);
		printf("phase_A_rms_current %f\n", pMsg_body->phase_A_rms_current);
		printf("phase_A_power_factor %f\n", pMsg_body->phase_A_power_factor);
		printf("phase_A_active_power %f\n", pMsg_body->phase_A_active_power);
		printf("phase_A_reactive_power %f\n", pMsg_body->phase_A_reactive_power);
		printf("phase_A_app_power %f\n", pMsg_body->phase_A_app_power);
		printf("Avg_Avg_phase_A_active_energy %f\n", pMsg_body->phase_A_active_energy);
		printf("phase_A_reactive_energy %f\n", pMsg_body->phase_A_reactive_energy);
		printf("phase_A_app_energy %f\n", pMsg_body->phase_A_app_energy);
		printf("phase_A_reverse_active_energy %f\n", pMsg_body->phase_A_reverse_active_energy);
		printf("phase_A_reverse_reactive_energy %f\n", pMsg_body->phase_A_reverse_reactive_energy);
		printf("phase_A_reverse_app_energy %f\n", pMsg_body->phase_A_reverse_app_energy);
		printf("\n");
		printf("phase_B_line_frequency %f\n", pMsg_body->phase_B_line_frequency);
		printf("phase_B_rms_voltage %f\n", pMsg_body->phase_B_rms_voltage);
		printf("phase_B_rms_current %f\n", pMsg_body->phase_B_rms_current);
		printf("phase_B_power_factor %f\n", pMsg_body->phase_B_power_factor);
		printf("phase_B_active_power %f\n", pMsg_body->phase_B_active_power);
		printf("phase_B_reactive_power %f\n", pMsg_body->phase_B_reactive_power);
		printf("phase_B_app_power %f\n", pMsg_body->phase_B_app_power);
		printf("phase_B_active_energy %f\n", pMsg_body->phase_B_active_energy);
		printf("phase_B_reactive_energy %f\n", pMsg_body->phase_B_reactive_energy);
		printf("phase_B_app_energy %f\n", pMsg_body->phase_B_app_energy);
		printf("phase_B_reverse_active_energy %f\n", pMsg_body->phase_B_reverse_active_energy);
		printf("phase_B_reverse_reactive_energy %f\n", pMsg_body->phase_B_reverse_reactive_energy);
		printf("phase_B_reverse_app_energy %f\n", pMsg_body->phase_B_reverse_app_energy);
		printf("\n");
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(meterTelemetryData_st) + i]);
		}
		printf("]\n");
//		printf("meter_update_no %d\n", pMsg_body->meter_update_no);
//		printf("period %d\n", pMsg_body->period);
//		printf("ph0_Q1_act_energy %lld\n", pMsg_body->ph0_Q1_act_energy);
//		printf("ph0_Q1_app_energy %lld\n", pMsg_body->ph0_Q1_app_energy);
//		printf("ph0_Q1_reactive_energy %lld\n", pMsg_body->ph0_Q1_reactive_energy);
//		printf("ph0_Q2_act_energy %lld\n", pMsg_body->ph0_Q2_act_energy);
//		printf("ph0_Q2_app_energy %lld\n", pMsg_body->ph0_Q2_app_energy);
//		printf("ph0_Q2_reactive_energy %lld\n", pMsg_body->ph0_Q2_reactive_energy);
//		printf("ph0_Q3_act_energy %lld\n", pMsg_body->ph0_Q3_act_energy);
//		printf("ph0_Q3_app_energy %lld\n", pMsg_body->ph0_Q3_app_energy);
//		printf("ph0_Q3_reactive_energy %lld\n", pMsg_body->ph0_Q3_reactive_energy);
//		printf("ph0_Q4_act_energy %lld\n", pMsg_body->ph0_Q4_act_energy);
//		printf("ph0_Q4_app_energy %lld\n", pMsg_body->ph0_Q4_app_energy);
//		printf("ph0_Q4_reactive_energy %lld\n", pMsg_body->ph0_Q4_reactive_energy);
//		printf("ph0_Q5_act_energy %lld\n", pMsg_body->ph0_Q5_act_energy);
//		printf("ph0_Q5_app_energy %lld\n", pMsg_body->ph0_Q5_app_energy);
//		printf("ph0_Q5_reactive_energy %lld\n", pMsg_body->ph0_Q5_reactive_energy);
//		printf("ph1_Q1_act_energy %lld\n", pMsg_body->ph1_Q1_act_energy);
//		printf("ph1_Q1_app_energy %lld\n", pMsg_body->ph1_Q1_app_energy);
//		printf("ph1_Q1_reactive_energy %lld\n", pMsg_body->ph1_Q1_reactive_energy);
//		printf("ph1_Q2_act_energy %lld\n", pMsg_body->ph1_Q2_act_energy);
//		printf("ph1_Q2_app_energy %lld\n", pMsg_body->ph1_Q2_app_energy);
//		printf("ph1_Q2_reactive_energy %lld\n", pMsg_body->ph1_Q2_reactive_energy);
//		printf("ph1_Q3_act_energy %lld\n", pMsg_body->ph1_Q3_act_energy);
//		printf("ph1_Q3_app_energy %lld\n", pMsg_body->ph1_Q3_app_energy);
//		printf("ph1_Q3_reactive_energy %lld\n", pMsg_body->ph1_Q3_reactive_energy);
//		printf("ph1_Q4_act_energy %lld\n", pMsg_body->ph1_Q4_act_energy);
//		printf("ph1_Q4_app_energy %lld\n", pMsg_body->ph1_Q4_app_energy);
//		printf("ph1_Q4_reactive_energy %lld\n", pMsg_body->ph1_Q4_reactive_energy);
//		printf("ph1_Q5_act_energy %lld\n", pMsg_body->ph1_Q5_act_energy);
//		printf("ph1_Q5_app_energy %lld\n", pMsg_body->ph1_Q5_app_energy);
//		printf("ph1_Q5_reactive_energy %lld\n", pMsg_body->ph1_Q5_reactive_energy);
		printf("\n");
	}
	if(msg_code == GET_REMOTE_HANDLE_POSISTION)
	{
		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("Handle status %d\n", pMsg_body[10]);
		printf("\n");
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		printf("]\n");
	}
	if(msg_code == SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER)
	{
		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("ACK_NACK %d\n", pMsg_body[10]);
		printf("\n");
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		printf("]\n");
	}
	if(msg_code == SET_LED_TO_USER_DEFINED_COLOUR)
	{
		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("ACK_NACK %d\n", pMsg_body[10]);
		printf("\n");
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		printf("]\n");
	}
	if(msg_code == SET_REMOTE_HANDLE_POSISTION)
	{

		printf("********Message Body*********\n");
		setHandlePosition_st *pMsg_body = &pRsponce->message_data;
		printf("ACK_NACK %d\n", pMsg_body->responce);
		printf("handle position %x\n", pMsg_body->handle_posistion);
		printf("\n");
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(setHandlePosition_st) + i]);
		}
		printf("]\n");
	}
	if(msg_code == SET_BREAKER_EOL_CALIB_PARAM)
	{

		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("ACK_NACK %d\n", pMsg_body[10]);
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(BreakerEOLCalibration_st) + i]);
		}
		printf("]\n");
		printf("\n");
	}
	if(msg_code == GET_BREAKER_EOL_CALIB_PARAM)
	{
		BreakerEOLCalibration_st *pMsg_body = &pRsponce->message_data;
		printf("********Message Body*********\n");

		printf("Phase_A voltage Gain:-[%x]\n", pMsg_body->avGain);
		printf("Phase_A Current Gain:-[%x]\n", pMsg_body->aiGain);
		printf("Phase_A RMS current OS:-[%x]\n", pMsg_body->aiRmsOs);
		printf("Phase_A Power Gain:-[%x]\n",pMsg_body->apGain);
		printf("Phase_A cal 0:-[%x]\n", pMsg_body->aphCal_0);

		printf("Phase_B voltage Gain:-[%x]\n", pMsg_body->bvGain);
		printf("Phase_B Current Gain:-[%x]\n", pMsg_body->biGain);
		printf("Phase_B RMS current OS:-[%x]\n", pMsg_body->biRmsOs);
		printf("Phase_B Power Gain:-[%x]\n",pMsg_body->bpGain);
		printf("Phase_B cal 0:-[%x]\n", pMsg_body->bphCal_0);

		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(BreakerEOLCalibration_st) + i]);
		}
		printf("]\n");
		printf("\n");
	}
	if(msg_code == SET_MANUFACTURING_MODE_ON)
	{

		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("ACK_NACK %d\n", pMsg_body[10]);
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		printf("]\n");
	}
	if(msg_code == SET_MANUFACTURING_MODE_OFF)
	{

		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("ACK_NACK %d\n", pMsg_body[10]);
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		printf("]\n");
	}
	if(msg_code == RESET_ENERGY)
	{

		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("ACK_NACK %d\n", pMsg_body[10]);
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		printf("]\n");
	}
	if(msg_code == GET_PRIMARY_HANDLE_STATUS)
	{

		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("Primary Handle status %d\n", pMsg_body[10]);
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		printf("]\n");
	}
	if(msg_code == SET_MANUFACTURING_RESET)
	{

		printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		printf("ACK_NACK %d\n", pMsg_body[10]);
		printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		printf("]\n");
	}

}
#endif
