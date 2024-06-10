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
#define MAX_SEQUENCE_NUMBER			0xFFFFFFFF
#define BORADCAST_KEY				1
#define UNICAST_KEY					0

#define BORADCAST_MESSAGE			1
#define UNICAST_MESSAGE				0
#define INVALID_MESSAGE				2
#define MINIMUM_MESSAGE_LENGTH		42

#define GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER	0x0000
#define GET_DEVICE_STATUS						0xFF00
#define GET_REMOTE_HANDLE_POSISTION				0x0001
#define GET_METER_TELEMETRY_DATA				0x0002
#define GET_PRIMARY_HANDLE_STATUS				0x0003
#define IDENTIFY_ME								0x0004
#define GET_MAC_ADDRESS							0x0005
#define GET_TRIP_LOG							0x0006
#define SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER	0x0080
#define SET_REMOTE_HANDLE_POSISTION				0x0081
#define SET_DEVICE_LOCK							0x0082
#define SET_LED_TO_USER_DEFINED_COLOUR			0x0083
#define SET_BREAKER_EOL_CALIB_PARAM             0x0084
#define GET_BREAKER_EOL_CALIB_PARAM				0x0085
#define RESET_ENERGY							0x0086
#define SET_MANUFACTURING_RESET					0x0087
#define SET_PRODUCTION_DETAILS					0x0088
#define GET_PRODUCTION_DETAILS					0x0089
#define SET_PCB_DETAILS							0x0090
#define SET_MANUFACTURING_MODE_ON				0x0091
#define SET_MANUFACTURING_MODE_OFF				0x0092

#define UNKNOWN_COMMAND							0xFFFF

#define SEVEN_DAYS_SECOND_COUNT			604800	//604800 sec = 7 days
#define EPOCH_TIME_Jan_1_2023           1672549200
#define UDP_KEY_TTL_DAYS				168	//168 hours = 7 Days
#define ONE_SEC							10	//100 ms * 10 = 1 sec
#define TWO_SEC							20	//100 ms * 20 = 2 sec
#define TEN_SEC							100	//100 ms * 100 = 10 sec
#define ONE_MIN							600	//100 ms * 100 = 10 sec
#define ONE_HOUR_COUNT					3600000 //3600000 ms = 1 hour

//following counts are used only for testing
//#define UDP_KEY_TTL_MINS				30 // 5 min //12	//120 sec / 10 = 2 mins for testing
#define UDP_KEY_TTL_MINS				10 // 10 min
//#define TWO_MIN_SECOND_COUNT			120	//604800 sec = 7 days
#define TEN_MIN_SECOND_COUNT			600	//600 sec = 10 min


#define IP_ADDRESS_SEPARATOR "." //This will be used for separate out the IP address fields
#define BROADCAST_IP_ADD_FILED				3
#define HEX_TO_ASCII_CONV_FOR_ZERO	0x30

#define MAX_CURRENT_RATING			60 // 60 Amps
#define MIN_CURRENT_RATING			15 //15 AMPS

static const char *TAG = "example";

static uint32_t g_sequenceNumber;
static uint32_t g_getNextUDPSeqNumber;
sblcpStatusInfo_st replyStatusInfo;
//static uint8_t g_SblcpFactoryMode;
primaryUnicastUdpKey_st g_primaryUnicastKeyStatus;
secondaryUnicastUdpKey_st g_secondaryUnicastKeyStatus;
primaryBroadcastUdpKey_st g_primaryBroadcastKeyStatus;
SecondaryBroadcastUdpKey_st g_secondaryBroadcastKeyStatus;

static uint32_t g_primaryUnicastKeyTtlTime;
static uint32_t g_secondaryUnicastKeyTtlTime;
static uint32_t g_primaryBroadcastKeyTtlTime;
static uint32_t g_secondaryBroadcastKeyTtlTime;

static uint8_t g_messageType;

static activeUDPkey_en g_unicastActiveKey;
static activeUDPkey_en g_broadcastActiveKey;
static uint8_t g_breakerCurrentRating;

uint32_t gf_data;
uint32_t hall_data_adc;

static uint32_t g_TtlHourCount = 0;

uint8_t gStm_Firmware_Version[16] = {0};//major.minor.patch versions

//--------------------------------------------------------------------------------------------------
//                    External (Visible) Function Prototypes
//--------------------------------------------------------------------------------------------------

extern DRAM_ATTR Device_Info_str DeviceInfo;

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
    uint8_t rx_buffer[256];
    char udp_client_str[128];
    int socket_type = (int)pvParameters;
    int ip_protocol = 0;
    int max_soc_handle;
    struct sockaddr_in6 udp_server_addr;


	uint32_t in_start_Of_mesg = 0;
	uint32_t in_seq_number    = 0;
	uint16_t in_message_code  =  0;
	uint16_t msg_data_size;
	uint8_t Ack_Nack_response = DONT_CARE;
	sblcpResponce_st *pReply = NULL;
	int16_t reply_length = 0;
	fd_set rfds;

	memset(&udp_server_addr, 0, sizeof(struct sockaddr_in6));

	if (socket_type == AF_INET)
	{
		struct sockaddr_in *udp_server_addr_ip4 = (struct sockaddr_in *)&udp_server_addr;
		udp_server_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
		udp_server_addr_ip4->sin_family = AF_INET;
		udp_server_addr_ip4->sin_port = htons(SBLCP_UDP_PORT);
		ip_protocol = IPPROTO_IP;
		ets_printf("Ip Address %d\n", udp_server_addr_ip4->sin_addr.s_addr);
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
		ets_printf("Unable to create socket \n");
	}
	else
	{
		ets_printf("Socket created \n");
		int err = bind(socket_handle, (struct sockaddr *)&udp_server_addr, sizeof(udp_server_addr));
		if (err < 0)
		{
			ets_printf("Socket unable to bind\n");
		}
		ets_printf("Socket bound, port %d \n\n", SBLCP_UDP_PORT);
	}

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

		if((DeviceInfo.bSblcpInit) && (!DeviceInfo.WiFiConnected) && (IsDeviceInProvisioningMode()))
		{
			//device is un-provisioned SBLCP will not work as
			//Wi-Fi Credentials will not be available; delete the SBLCP task
			//SBLCP will re initialize again when credentials are available
			//exit from while loop to delete the task
			DeviceInfo.bSblcpInit = false;
			break;
		}
/****************Check UDP key Validity *************************/
		//UDP keys are used and checked for validity only in User mode.
		//if(g_SblcpFactoryMode == FACTORY_MODE_DISABLE)
		if(DeviceInfo.DeviceInFactoryMode == FACTORY_MODE_DISABLE)
		{
#if SBLCP_UDP_KEY_DEBUG
				if(g_TtlHourCount == ONE_MIN)
#else
				if(g_TtlHourCount == ONE_HOUR_COUNT)
#endif
				{
					//if any any key is available then only check for key validity, else reset the counter.
					if((!g_primaryUnicastKeyStatus.primary_unicast_key_expire) || (!g_secondaryUnicastKeyStatus.secondary_unicast_key_expire) ||
							(!g_primaryBroadcastKeyStatus.primary_broadcast_key_expire) || (!g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire))
					{
						g_TtlHourCount = 0;
						//check validity of all UDP keys
						checkUdpKeyExpiry();
						checkActiveUnicastKey();
						checkActiveBroadcastKey();
					}
					else
					{
						g_TtlHourCount = 0;
					}
				}
				else
				{
					//increments every 100 ms for 1 hour check.
					g_TtlHourCount++;
				}
		}
/***********************************************************/
		FD_SET(socket_handle, &rfds);
		if (select(max_soc_handle, &rfds, NULL, NULL, &to) < 0)
		{
			ets_printf("Select error\n");
			perror("select()");
		}
		else
		{
			if(FD_ISSET(socket_handle, &rfds))
			{
				memset(rx_buffer, 0x00, sizeof(rx_buffer));
#if SBLCP_DEBUG
				ets_printf("################## Waiting for data #####################\n");
#endif
				int len = recvfrom(socket_handle, rx_buffer, (sizeof(rx_buffer) - 1), 0, (struct sockaddr *)&source_addr, &socklen);
				if (len < MINIMUM_MESSAGE_LENGTH)
				{
					ets_printf("recvfrom failed, Invalid message received\n");
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

					//extract the data from received packet from client
					in_start_Of_mesg = EXTRACT_DATA_4_BYTE(rx_buffer, 0);
					in_seq_number = EXTRACT_DATA_4_BYTE(rx_buffer, 4);
					in_message_code = EXTRACT_DATA_2_BYTE(rx_buffer, 8);
#if SBLCP_DEBUG
					ets_printf("\nReceived %d bytes from %s:\n",len, udp_client_str);
					ets_printf("received start byte %x;\nreceived seq.number %x;\nreceived mesg code %x \n", in_start_Of_mesg, in_seq_number, in_message_code);
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
						ets_printf("GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER received in less than two second \n");
#endif
					}
					else
					{
						replyStatusInfo.previous_received_command = in_message_code;
						replyStatusInfo.timeout = 0;
						if(in_start_Of_mesg == CO_ORDINATOR_START_MESSAGE)
						{

#if SBLCP_DEBUG
							ets_printf("start of Message byte matches \n");
#endif
							if(verify_hash(rx_buffer, in_message_code, len) == SBLCP_SUCCESS)
							{
#if SBLCP_DEBUG
								ets_printf("Cryptographic hash check pass \n");
#endif
								if(checkSequenceNumber(in_seq_number, in_message_code) == SBLCP_SUCCESS)
								{
#if SBLCP_DEBUG
									ets_printf("sequence number check pass \n");
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
													ets_printf("Error occurred during sending \n");
												}
											}
											free(pReply);
										}
									}

								}
								else
								{
									ets_printf("sequence number check fails \n");
									updateSequenceNumber();
								}
							}
							else
							{
								ets_printf("Cryptographic hash check fails \n");
								updateSequenceNumber();
							}
						}//message code check
						else
						{
							ets_printf("Start of message byte dose not match \n");
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
						ets_printf("@@@@@ 11 SBLCP_ERROR :: responce memory not allocated @@@@@\n");
					}

					if(replyStatusInfo.timeout == TWO_SEC)
					{
						//if code comes here means status is not updated, response is sent with negative acknowledge
#if SBLCP_DEBUG
						ets_printf("Sending Negative ACK\n");
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
							ets_printf("Error occurred during sending \n");
						}
						replyStatusInfo.timeout = 0;
						replyStatusInfo.sblcp_timeout_command = false;
						replyStatusInfo.status_updated = false;
					}
					free(pReply);

				}
				replyStatusInfo.timeout ++;
			}//else FD_ISSET
		}// socket select else
	}//while(1)

	if (socket_handle != -1)
	{
		ESP_LOGE(TAG, "Shutting down socket and restarting...");
		shutdown(socket_handle, 0);
		close(socket_handle);
	}
	ets_printf("Exiting the SBLCP Task\n");
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
bool init_SBLCP()
{
	int8_t err = SBLCP_SUCCESS;
 
	if(init_sequence_number() == SBLCP_SUCCESS)
	{
		BaseType_t task_error;
 
		task_error = xTaskCreate(sblcp_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
 
		if(task_error == pdPASS)
		{
			//if(g_SblcpFactoryMode != FACTORY_MODE_DISABLE)
			if(DeviceInfo.DeviceInFactoryMode != FACTORY_MODE_DISABLE)
			{
#if SBLCP_DEBUG
				ets_printf("Factory Mode is Enable\n");
#endif
//				SBLCP_resetUdpKeys();
				replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_ON;

				//if metro task is running then only set the sampling time.
				if(DeviceInfo.MetroInitStatus)
				{
					//if default manufacturing mode is ON, set metro sampling rate to 50ms
					// it is set to 50ms at SBLCP init to reduce the the calibration time
					// it set to normal sampling rate when manufacturing mode is set to OFF
					Metro_Set_Sample_Read_time();
				}

			}
			else
			{
#if SBLCP_DEBUG
				ets_printf("Factory Mode is Disable\n");
#endif
				replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_OFF;
				//UDP keys are used only USER mode i.e. when Factory Mode is Disabled.
/*****************************/
				//check primary unicast expiry and TTL
				if(g_primaryUnicastKeyStatus.primary_unicast_key_expire)
				{
					g_primaryUnicastKeyTtlTime = 0;
					ets_printf("No valid Primary Uni-cast key available, update uni-cast key\n");
				}
				else
				{
					read_PrimaryUnicastUdpKey();
				}
/*******************************/
				//check primary broadcast key expiry and TTL
				if(g_primaryBroadcastKeyStatus.primary_broadcast_key_expire)
				{
					g_primaryBroadcastKeyTtlTime = 0;
					ets_printf("No valid Primary Broadcast key available, update Broadcast key\n");
				}
				else
				{
					read_PrimaryBroadcasttUdpKey();
				}
/********************************/
				//check secondary unicast expiry and TTL
				if(g_secondaryUnicastKeyStatus.secondary_unicast_key_expire)
				{
					g_secondaryUnicastKeyTtlTime = 0;
					ets_printf("No valid Secondary Uni-cast key available, update uni-cast key\n");
				}
				else
				{
					read_secondaryUnicastUdpKey();
				}
/*******************************/
				//check secondary broadcast key expiry and TTL
				if(g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire)
				{
					g_secondaryBroadcastKeyTtlTime = 0;
					ets_printf("No valid Secondary Broadcast key available, update Broadcast key\n");
				}
				else
				{
					read_secondaryBroadcasttUdpKey();
				}
				checkActiveUnicastKey();
				checkActiveBroadcastKey();
#if SBLCP_DEBUG
				ets_printf("\n");
				ets_printf("Primary unicast Key %s \n", DeviceInfo.UnicastPrimaryUDPKey);
				ets_printf("Primary Unicast TTL %d \n", g_primaryUnicastKeyTtlTime);
				ets_printf("Primary Unicast key download time %d \n", g_primaryUnicastKeyStatus.primary_unicast_key_download_time);
				ets_printf("Primary Unicast key expiry time %d \n", g_primaryUnicastKeyStatus.primary_unicast_key_expiry_time);
				ets_printf("\n");
				ets_printf("Primary Broadcast Key %s \n", DeviceInfo.BroadcastPrimaryUDPKey);
				ets_printf("Primary broadcast TTL %d \n", g_primaryBroadcastKeyTtlTime);
				ets_printf("Primary Unicast key download time %d \n", g_primaryBroadcastKeyStatus.primary_broadcast_key_download_time);
				ets_printf("Primary Unicast key expiry time %d \n", g_primaryBroadcastKeyStatus.primary_broadcast_key_expiry_time);
				ets_printf("\n");
				ets_printf("secondary unicast Key %s \n", DeviceInfo.UnicastSecondaryUDPKey);
				ets_printf("secondary Unicast TTL %d \n", g_secondaryUnicastKeyTtlTime);
				ets_printf("secondary Unicast key download time %d \n", g_secondaryUnicastKeyStatus.secondary_unicast_key_download_time);
				ets_printf("secondary Unicast key expiry time %d \n", g_secondaryUnicastKeyStatus.secondary_unicast_key_expiry_time);
				ets_printf("\n");
				ets_printf("secondary Broadcast Key %s \n", DeviceInfo.BroadcastSecondaryUDPKey);
				ets_printf("secondary broadcast TTL %d \n", g_secondaryBroadcastKeyTtlTime);
				ets_printf("secondary Unicast key download time %d \n", g_secondaryBroadcastKeyStatus.secondary_broadcast_key_download_time);
				ets_printf("secondary Unicast key expiry time %d \n", g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expiry_time);
				ets_printf("\n");
#endif
			}

			replyStatusInfo.status_updated = false;
			replyStatusInfo.sblcp_timeout_command = false;
			replyStatusInfo.timeout = 0;
			//at power on previous command is set to unknown
			replyStatusInfo.previous_received_command = UNKNOWN_COMMAND;

			ets_printf("*********SBLCP task init Success*********\n");
		}
		else
		{
			err = false;
		}
	}
	else
	{
		err = false;
	}
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
				ets_printf("received Sequence number is out of range\n");
				err = SBLCP_ERROR;
			}
		}
		else
		{
			ets_printf("received and current Sequence match fails\n");
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
			ets_printf("preparing reply for GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER\n");
#endif
			getNextUDPSeqNoReply(psblcp_msg, prx_buffer);
			updateSequenceNumber();
		}
		break;
		case SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER\n");
#endif
			setSequenceNumber(prx_buffer, psblcp_msg);
			updateSequenceNumber();
		}
		break;
		case GET_DEVICE_STATUS:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for GET_DEVICE_STATUS\n");
#endif
			getBreakerStatus((breakerStatus_st *)psblcp_msg, prx_buffer);
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
					//if metro task is running then only start the averaging of metro data
					if(DeviceInfo.MetroInitStatus)
					{
						Metro_start_data_average();
					}
					replyStatusInfo.sblcp_timeout_command = true;
				}
				else
				{
#if SBLCP_DEBUG
			ets_printf("preparing reply for GET_METER_TELEMETRT_DATA\n");
#endif
 					SBLCP_getSetAvgTeleData((meterTelemetryData_st *)psblcp_msg);
					updateSequenceNumber();
				}

			}
			else
			{
#if SBLCP_DEBUG
			ets_printf("preparing reply for GET_METER_TELEMETRT_DATA\n");
#endif
				Metro_Get_Telemetry_data((meterTelemetryData_st *)psblcp_msg);
				updateSequenceNumber();
			}
		}
		break;
		case GET_REMOTE_HANDLE_POSISTION:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for GET_REMOTE_HANDLE_POSISTION\n");
#endif
			*psblcp_msg = DeviceInfo.SecondaryContactState;
			updateSequenceNumber();
		}
		break;
		case GET_PRIMARY_HANDLE_STATUS:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for GET_PRIMARY_HANDLE_STATUS\n");
#endif
			*psblcp_msg = DeviceInfo.PrimaryContactState;
			updateSequenceNumber();
		}
		break;
		case SET_REMOTE_HANDLE_POSISTION:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_REMOTE_HANDLE_POSISTION\n");
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
				if(response == NACK)
				{
					if((DeviceInfo.SecondaryContactState == SS_OPENING) || (DeviceInfo.SecondaryContactState == SS_CLOSING))
					{
						secondary_handle_status.responce = ACK;
					}
				}
				secondary_handle_status.handle_posistion = DeviceInfo.SecondaryContactState;
				memcpy(psblcp_msg, &secondary_handle_status, sizeof(setHandlePosition_st));
				updateSequenceNumber();
			}
		}
		break;
		case SET_LED_TO_USER_DEFINED_COLOUR:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_LED_TO_USER_DEFINED_COLOUR\n");
#endif
			setLedColour_st led_state;
			memcpy(&led_state, prx_buffer + sizeof(sblcpHeader_st) ,sizeof(setLedColour_st));

			BlinkLEDs(led_state.blinking_time_interval,led_state.blinking_time_expire,(LED_STATE)led_state.start_blinking_leds_flag);
			SetRGB_Color((RGB_COLOR)led_state.led_colour);
			*psblcp_msg = ACK;
			updateSequenceNumber();
		}
		break;
		case SET_BREAKER_EOL_CALIB_PARAM:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_BREAKER_EOL_CALIB_PARAM\n");
#endif
			if(!replyStatusInfo.status_updated)
			{
				replyStatusInfo.sblcp_timeout_command = true;
				BreakerEOLCalibration_st *calibData = malloc(sizeof(BreakerEOLCalibration_st));
				if(calibData == NULL)
				{
					iRet = SBLCP_ERROR;
				}
				else
				{
					memcpy(calibData, prx_buffer + sizeof(sblcpHeader_st), sizeof(BreakerEOLCalibration_st));
					Metro_Set_Breaker_Calibration_Factors((uint8_t *)calibData);
				}
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
			ets_printf("preparing reply for GET_BREAKER_EOL_CALIB_PARAM\n");
#endif
			BreakerEOLCalibration_st calib_factors;
			size_t length;

			memset(&calib_factors, 0, sizeof(BreakerEOLCalibration_st));

			length = ReadDataFromFlash(EEOFFSET_METRO_CALIB_LABEL, (char *)&calib_factors);
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
			ets_printf("preparing reply for SET_MANUFACTURING_MODE_ON\n");
#endif
			*psblcp_msg = ACK;
			replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_ON;
			//if metro task is running then only set the sampling time.
			if(DeviceInfo.MetroInitStatus)
			{
				Metro_Set_Sample_Read_time();
			}
			updateSequenceNumber();
		}
		break;
		case SET_MANUFACTURING_MODE_OFF:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_MANUFACTURING_MODE_OFF\n");
#endif
			*psblcp_msg = ACK;
			replyStatusInfo.manufacturing_Mode = MANUFACTURING_MODE_OFF;
			//if metro task is running then only set the sampling time.
			if(DeviceInfo.MetroInitStatus)
			{
				Metro_Set_Sample_Read_time();
			}
			updateSequenceNumber();
		}
		break;
		case RESET_ENERGY:
		{
			int8_t err;
#if SBLCP_DEBUG
			ets_printf("preparing reply for RESET_ENERGY\n");
#endif
			*psblcp_msg = ACK;

			EERAM_Reset_Energy();

			updateSequenceNumber();
		}
		break;
		case SET_MANUFACTURING_RESET:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_MANUFACTURING_RESET\n");
#endif
			*psblcp_msg = NACK;
			SBLCP_performFactoryReset();
			updateSequenceNumber();
		}
		break;
		case SET_PRODUCTION_DETAILS:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_PRODUCTION_DETAILS\n");
#endif
			//update the production data to device info structure and store the
			//data to ESP NVS
			*psblcp_msg = updateProductionDetails(prx_buffer);
			updateSequenceNumber();
		}
		break;
		case GET_PRODUCTION_DETAILS:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for GET_PRODUCTION_DETAILS\n");
#endif
			DeviceProductionDetails_st prod_details;

			memset(&prod_details, 0, sizeof(DeviceProductionDetails_st));
			//get all the production specific data
			getProductionDetails(&prod_details);

			memcpy(psblcp_msg, &prod_details, sizeof(DeviceProductionDetails_st));
			updateSequenceNumber();

		}
		break;
		case SET_PCB_DETAILS:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_PCB_DETAILS\n");
#endif
			//update the PCB details to device info structure and store the
			//data to ESP NVS
			*psblcp_msg = updatePcbDetails(prx_buffer);
			updateSequenceNumber();
		}
		break;
		case SET_DEVICE_LOCK:
		{
			int err = SBLCP_SUCCESS;

#if SBLCP_DEBUG
			ets_printf("preparing reply for SET_DEVICE_LOCK\n");
#endif
			err = lockSB2Device();
			if(err == SBLCP_ERROR)
			{
#if SBLCP_DEBUG
				printf("Device is not Locked \n");
#endif
				//Below code is temporary code, it will be removed in production code.
				//Its added to make it testable internally.
#if SBLCP_TEMP_CODE
				SBLCP_performFactoryReset();
				WriteByteToFlash(EEOFFSET_SBLCP_FACTORY_MODE,FACTORY_MODE_DISABLE);
#endif
				*psblcp_msg = NACK;
			}
			else
			{
				SBLCP_performFactoryReset();
#if SBLCP_DEBUG
				ets_printf("Disabling the Factory Mode\n");
#endif
				WriteByteToFlash(EEOFFSET_SBLCP_FACTORY_MODE,FACTORY_MODE_DISABLE);
				*psblcp_msg = ACK;
			}
			updateSequenceNumber();
		}
		break;
		case IDENTIFY_ME:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for IDENTIFY_ME\n");
#endif
			//Blinks the white LED when this command is received as self identification
			IdentifyMeBegin();
			*psblcp_msg = ACK;
			updateSequenceNumber();
		}
		break;
		case GET_MAC_ADDRESS:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for GET_MAC_ADDRESS\n");
#endif
			memset(psblcp_msg, 0 , sizeof(DeviceInfo.MacAddress));
			memcpy(psblcp_msg, DeviceInfo.MacAddress, sizeof(DeviceInfo.MacAddress));
			updateSequenceNumber();
		}
		break;
		case GET_TRIP_LOG:
		{
#if SBLCP_DEBUG
			ets_printf("preparing reply for GET_TRIP_LOG\n");
#endif
			memset(psblcp_msg, 0 , sizeof(ErrorLog_st));
			get_TripLog(psblcp_msg);
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
	else if(in_message_code ==  SET_PRODUCTION_DETAILS)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code ==  GET_PRODUCTION_DETAILS)
	{
		msg_size = sizeof(DeviceProductionDetails_st);
	}
	else if(in_message_code ==  SET_PCB_DETAILS)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code ==  SET_DEVICE_LOCK)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code ==  IDENTIFY_ME)
	{
		msg_size = sizeof(uint8_t);
	}
	else if(in_message_code ==  GET_MAC_ADDRESS)
	{
		msg_size = sizeof(DeviceInfo.MacAddress);
	}
	else if(in_message_code ==  GET_TRIP_LOG)
	{
		msg_size = sizeof(ErrorLog_st);
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
 * @brief		Function prepares the respose for the received command
 *
 * @param[in] 	1. received data; 2. received message code; 3.pointer for response; 4. ACK/NACK response
 * @retval    SBLCP_SUCCESS / SBLCP_ERROR
*/
//--------------------------------------------------------------------------------------------------
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
		//if(g_SblcpFactoryMode == FACTORY_MODE_DISABLE)
		if(DeviceInfo.DeviceInFactoryMode == FACTORY_MODE_DISABLE)
		{
			// SET_MANUFACTURING_MODE_ON and SET_MANUFACTURING_MODE_ON should be processed if Factory mode is disable
			if((in_message_code == SET_MANUFACTURING_MODE_ON) || (in_message_code == SET_MANUFACTURING_MODE_OFF) || (in_message_code == SET_DEVICE_LOCK))
			{
				ets_printf("Can not access this command, Factory mode is Disable\n");
				iRet = SBLCP_ERROR;
				return iRet;
			}
		}
		//commands which dose not support manufacturing mode do not process.
		if((replyStatusInfo.manufacturing_Mode == MANUFACTURING_MODE_OFF) &&
				((in_message_code == SET_BREAKER_EOL_CALIB_PARAM) || (in_message_code == GET_BREAKER_EOL_CALIB_PARAM) ||
				(in_message_code == SET_PRODUCTION_DETAILS) || (in_message_code == SET_LED_TO_USER_DEFINED_COLOUR) || (in_message_code == SET_PCB_DETAILS) ||
				(in_message_code == SET_DEVICE_LOCK) || (in_message_code == GET_MAC_ADDRESS)))
		{
			//metro calibration should be done only if manufacturing mode is on
			ets_printf("Manufacturing mode is OFF, Turn ON Manufacturing Mode\n");
			iRet = SBLCP_ERROR;
		}
		else
		{
			//get the pointer for response body
			pMsgPtr = pReply + sizeof(pReply_Msg->sblcp_header);

			// get the pointer for crypto has
			pHashPtr = pReply + msg_data_size + sizeof(pReply_Msg->sblcp_header);

			if(iRet > SBLCP_ERROR)
			{
				uint8_t message_body[1024] = {0};//array size needs to be limited
				//update the reponce header
				update_responce_header((sblcpResponce_st *)&heade_footer.sblcp_header, prx_buffer);
				//get the response into response body
				iRet = getMessage(in_message_code, prx_buffer, message_body, response);
				ets_printf("\n");

				memcpy(pReply, (uint8_t *)&heade_footer.sblcp_header, sizeof(heade_footer.sblcp_header));

				memcpy(pMsgPtr, message_body, msg_data_size);
				//calculate the crypto hash on header and message body
				iRet = calculate_hash(pReply, msgdata_len, heade_footer.sblcp_hash);
				if(iRet > SBLCP_ERROR)
				{
					memcpy(pHashPtr, heade_footer.sblcp_hash, sizeof(heade_footer.sblcp_hash));
				}
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
int8_t verify_hash(uint8_t *prx_buffer, uint16_t sblcp_msg_type, int length)
{
	unsigned char udp_key[CRYPTO_HASH_LENGTH + 1];
	//unsigned char msg_hex_to_string[1024];
	unsigned char cryptop_hmac_hash[MAX_HASHLEN] = {0};
	int8_t iRet = SBLCP_SUCCESS;
	uint8_t *pHashPtr = NULL;
	uint16_t msg_data_size;
	psSize_t udp_keyLen = MAX_HASHLEN;

	//length of the message is calculated on which
	//Hash must calculated for verification
	msg_data_size = length - CRYPTO_HASH_LENGTH;
	uint8_t message_data[msg_data_size];

	memset(message_data, 0, sizeof(message_data));

	//hexToString(msg_hex_to_string, message_data, msg_data_size);
#if SBLCP_DEBUG
		ets_printf("\message in integer \n");
		for(uint16_t i = 0; i < msg_data_size; i++)
		{
			ets_printf("%02x",message_data[i]);
		}
		ets_printf("\n");
		//ets_printf("stringMessage %s \n", msg_hex_to_string);
		//ets_printf("message length %d\n", strlen((char*)msg_hex_to_string));
#endif

	udp_key[CRYPTO_HASH_LENGTH] = '\0';
	memcpy(message_data, prx_buffer, msg_data_size);

	pHashPtr = prx_buffer + msg_data_size;

	//Hash calculation is done using both Unicast and broadcast key.
	//if calculated hash dose not matches with Unicast key, calculate hash using broadcast key.
	//IF Unicast key is used for hash calculation means Unicast message is received
	//if Broadcast message is received, measn broadcast message is received
/*************** check for unicast message ****************************/
	iRet = get_UdpKeyForHashCalculation(udp_key, UNICAST_KEY);
	if(iRet == SBLCP_SUCCESS)
	{
		//calculate has using unicast key
		if((psHmac(HMAC_SHA256, udp_key, udp_keyLen, message_data, msg_data_size, cryptop_hmac_hash)) == 0)
		{
			if(memcmp(pHashPtr, cryptop_hmac_hash, MAX_HASHLEN) == 0)
			{
				iRet = SBLCP_SUCCESS;
				setMesaageType(UNICAST_MESSAGE);
#if SBLCP_DEBUG
				ets_printf("Unicast udp_key used::[%s]\n", udp_key);
				ets_printf("HASH RECVD::\n");
				for(uint8_t i = 0; i < MAX_HASHLEN; i++)
				{
					ets_printf("%x ",pHashPtr[i]);
				}
				ets_printf("\n");

				ets_printf("HMAC_SHA256 DONE::\n");
				for(uint8_t i = 0; i < MAX_HASHLEN; i++)
				{
					ets_printf("%x ",cryptop_hmac_hash[i]);
				}
				ets_printf("\n");
				ets_printf("Both HASH MATCHED\n");
				ets_printf("Unicast message is received\n");
#endif
			}
			else
			{
				iRet = SBLCP_ERROR;
			}
		}
		else
		{
			iRet = SBLCP_ERROR;
		}
	}
	else
	{
#if SBLCP_DEBUG
		ets_printf("No Unicast key available\n");
#endif
	}
/*************** check for Broadcast message ****************************/
	if(iRet == SBLCP_ERROR)
	{
		memset(udp_key, 0, sizeof(udp_key));
		udp_key[CRYPTO_HASH_LENGTH] = '\0';
		memset(cryptop_hmac_hash, 0, sizeof(cryptop_hmac_hash));

		iRet = get_UdpKeyForHashCalculation(udp_key, BORADCAST_KEY);
		if(iRet == SBLCP_SUCCESS)
		{
			//calculate has using broadcast key
			if((psHmac(HMAC_SHA256, udp_key, udp_keyLen, message_data, msg_data_size, cryptop_hmac_hash)) == 0)
			{
				if(memcmp(pHashPtr, cryptop_hmac_hash, MAX_HASHLEN) == 0)
				{
					iRet = SBLCP_SUCCESS;
					setMesaageType(BORADCAST_MESSAGE);
#if SBLCP_DEBUG
					ets_printf("broadcast udp_key used::[%s]\n", udp_key);
					ets_printf("HASH RECVD::\n");
					for(uint8_t i = 0; i < MAX_HASHLEN; i++)
					{
						ets_printf("%x ",pHashPtr[i]);
					}
					ets_printf("\n");

					ets_printf("HMAC_SHA256 DONE::\n");
					for(uint8_t i = 0; i < MAX_HASHLEN; i++)
					{
						ets_printf("%x ",cryptop_hmac_hash[i]);
					}
					ets_printf("\n");
					ets_printf("\nBoth HASH MATCHED\n");
					ets_printf("Broadcast message is received\n");
#endif
				}
				else
				{
					iRet = SBLCP_ERROR;
				}
			}
			else
			{
				iRet = SBLCP_ERROR;
			}
		}
		else
		{
#if SBLCP_DEBUG
		ets_printf("No Broadcast key available\n");
#endif
		}
	}

#if DISABLE_HASH_VERIFICATION
	return SBLCP_SUCCESS;
#endif
	return iRet;
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
	//if((DeviceInfo.WiFiConnected) && ((g_SblcpFactoryMode != FACTORY_MODE_DISABLE) || (seconds > EPOCH_TIME_Jan_1_2023)))
	if((DeviceInfo.WiFiConnected) && ((DeviceInfo.DeviceInFactoryMode != FACTORY_MODE_DISABLE) || (seconds > EPOCH_TIME_Jan_1_2023)))
	{
		srand(seconds);
		g_sequenceNumber = rand();
		g_getNextUDPSeqNumber = g_sequenceNumber;
		ets_printf("\n***************init sequense_number 0x%x \n", g_sequenceNumber);
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
	uint8_t len;
	len = strlen(DeviceInfo.breaker_serial_no);
	if(len <= 0)
	{
		memset(replyMesg->serial_number, 0, (sizeof(DeviceInfo.breaker_serial_no) - 1));
	}
	else
	{
		memcpy(replyMesg->serial_number, DeviceInfo.breaker_serial_no, (sizeof(DeviceInfo.breaker_serial_no) - 1));
	}
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
	uint32_t ver = SBLCP_PROTOTYPE_VER;
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
int16_t calculate_hash(uint8_t *pMsgData, uint16_t msgdata_size, uint8_t *hash)
{
	unsigned char udp_key[CRYPTO_HASH_LENGTH + 1];
	psSize_t udp_keyLen = 32;
	uint8_t message_type;
	//unsigned char msg_hex_to_string[1024];
	int16_t err = 0;

	memset(udp_key, 0, sizeof(udp_key));
	message_type = getMesaageType();
	if(message_type == UNICAST_MESSAGE)
	{
		get_UdpKeyForHashCalculation(udp_key, UNICAST_MESSAGE);
	}
	else
	{
		get_UdpKeyForHashCalculation(udp_key, BORADCAST_MESSAGE);
	}

	udp_key[CRYPTO_HASH_LENGTH] = '\0';

	//hexToString(msg_hex_to_string, pMsgData, msgdata_size);

//#if SBLCP_DEBUG
//		ets_printf("\message in integer \n");
//		for(uint8_t i = 0; i < msgdata_size; i++)
//		{
//			ets_printf("%02x",pMsgData[i]);
//		}
//		ets_printf("\n");
////		ets_printf("stringMessage %s \n", msg_hex_to_string);
//#endif

	//HMAC SHA256 crypto has is calculated on message before sending it to SBLCP client
	//calculate the HASH on the message (Header + Message body)
	if ((psHmac(HMAC_SHA256, udp_key, udp_keyLen, pMsgData,  msgdata_size, hash)) == 0)
	{
#if SBLCP_DEBUG
			ets_printf("HMAC_SHA256 DON on Hex E::\n");
			//do common return path
			for(uint8_t i = 0; i < MAX_HASHLEN; i++)
			{
				ets_printf("%x,",hash[i]);
			}
			ets_printf("\n");
#endif
		err = SBLCP_SUCCESS;
	}
	else
	{
		err = SBLCP_ERROR;
	}

	return err;
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
	handle_posistion = EXTRACT_DATA_1_BYTE(rxBuff, 10);

	ets_printf("Requested handle position %d\n", handle_posistion);
	sblcpSetSecondaryHandle(handle_posistion);
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
	Metro_Get_Telemetry_data(metro_temetry_data);

	reply_message->telemetry_data.phase_B_Q4_app_energy = (uint64_t)gf_data;
	reply_message->telemetry_data.phase_B_Q5_app_energy = (uint64_t)hall_data_adc;

	//updates the secondary handle status
	reply_message->breaker_state = DeviceInfo.SecondaryContactState;
	reply_message->primary_handle_status = DeviceInfo.PrimaryContactState;
	reply_message->path_status = DeviceInfo.PathStatus;
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
		ets_printf("Sequence number exceeds the range \n");
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
	if(new_sequense_number > (g_getNextUDPSeqNumber + SEQUENCE_NUMBER_LIMIT) &&
			(new_sequense_number < (MAX_SEQUENCE_NUMBER - SEQUENCE_NUMBER_LIMIT)))
	{
		g_getNextUDPSeqNumber = new_sequense_number;
		g_sequenceNumber = new_sequense_number;
		*psblcp_msg = ACK;
	}
	else
	{
		*psblcp_msg = BAD_RESPONCE;
	}
}
void SBLCP_getSetAvgTeleData(meterTelemetryData_st *telemetry_data)
{
	static meterTelemetryData_st *Avg_data;
	if(Avg_data == NULL)
	{
		Avg_data = (meterTelemetryData_st*)malloc(sizeof(meterTelemetryData_st));
		if(Avg_data != NULL)
		{
			memset(Avg_data, 0, sizeof(meterTelemetryData_st));
			if(replyStatusInfo.status_updated)
			{
				//Code comes here if 2 sec timeout is hit, so no telemetry data has been updated
				//response must be updated with 0 values for all telemetry parameters.
				memcpy(telemetry_data, Avg_data, sizeof(meterTelemetryData_st));
				free(Avg_data);
				Avg_data = NULL;
#if SBLCP_DEBUG
				ets_printf("Timeout is hit, Telemetry data updated with zero\n");
#endif
			}
			else
			{
				memcpy(Avg_data, telemetry_data, sizeof(meterTelemetryData_st));
				replyStatusInfo.status_updated = true;
			}
		}
	}
	else
	{
		memcpy(telemetry_data, Avg_data, sizeof(meterTelemetryData_st));
#if SBLCP_DEBUG
		ets_printf("update metro for sending reply\n");
#endif
		free(Avg_data);
		Avg_data = NULL;
	}

}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_resetUdpKeys()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function resets the both UDP keys stored in NVS
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_resetUdpKeys()
{
	eeram_err_t ret = EERAM_SUCCESS;

	//at first power Up update NVS locations with zero
	//After factory reset UDP keys will be reset and set to zero
#if SBLCP_DEBUG
	ets_printf("Resetting all UDP keys\n");
#endif

	if(reset_primaryUnicastkey() != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR;
	}
	if(reset_secondaryUnicastkey() != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR;
	}
	if(reset_primaryBroadcastkey() != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR;
	}
	if(reset_secondaryBroadcastkey() != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR;
	}

	return ret;

}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        update_unicastPrimaryKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates new Primary unicast key and its status also stores it NVS
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void update_unicastPrimaryKey()
{
	g_primaryUnicastKeyStatus.primary_unicast_key_download_time = GetEpochTime();

#if SBLCP_UDP_KEY_DEBUG
	g_primaryUnicastKeyStatus.primary_unicast_key_expiry_time = g_primaryUnicastKeyStatus.primary_unicast_key_download_time + TEN_MIN_SECOND_COUNT;
	g_primaryUnicastKeyTtlTime = UDP_KEY_TTL_MINS; //10 min
#else
	g_primaryUnicastKeyStatus.primary_unicast_key_expiry_time = g_primaryUnicastKeyStatus.primary_unicast_key_download_time + SEVEN_DAYS_SECOND_COUNT;
	g_primaryUnicastKeyTtlTime = UDP_KEY_TTL_DAYS; //7 days
#endif
//	g_uniCastKey_TTL_hour_count = 0;

	g_primaryUnicastKeyStatus.primary_unicast_key_expire = false;

	SBLCP_HAL_EERAM_Write_unicastPrimaryKeyStatus((uint8_t *)&g_primaryUnicastKeyStatus, sizeof(g_primaryUnicastKeyStatus));
	SBLCP_HAL_EERAM_Write_unicastPrimaryKeyTtl(&g_primaryUnicastKeyTtlTime, sizeof(g_primaryUnicastKeyTtlTime));

	checkActiveUnicastKey();

#if SBLCP_DEBUG
	ets_printf("New Primary uni-cast UDP key %s is stored to NVS at %d\n", DeviceInfo.UnicastPrimaryUDPKey, g_primaryUnicastKeyStatus.primary_unicast_key_download_time);
	ets_printf("g_unicastKeyTtlTime %d\n", g_primaryUnicastKeyTtlTime);
#endif

}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        update_unicastSecondaryKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates new secondary unicast key and its status also stores it NVS
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void update_unicastSecondaryKey()
{
	g_secondaryUnicastKeyStatus.secondary_unicast_key_download_time = GetEpochTime();

#if SBLCP_UDP_KEY_DEBUG
	g_secondaryUnicastKeyStatus.secondary_unicast_key_expiry_time = g_secondaryUnicastKeyStatus.secondary_unicast_key_download_time + TEN_MIN_SECOND_COUNT;
	g_secondaryUnicastKeyTtlTime = UDP_KEY_TTL_MINS;
#else
	g_secondaryUnicastKeyStatus.secondary_unicast_key_expiry_time = g_secondaryUnicastKeyStatus.secondary_unicast_key_download_time + SEVEN_DAYS_SECOND_COUNT;
	g_secondaryUnicastKeyTtlTime = UDP_KEY_TTL_DAYS;
#endif

	g_secondaryUnicastKeyStatus.secondary_unicast_key_expire = false;

	SBLCP_HAL_EERAM_Write_unicastSecondaryKeyStatus((uint8_t *)&g_secondaryUnicastKeyStatus, sizeof(g_secondaryUnicastKeyStatus));
	SBLCP_HAL_EERAM_Write_unicastSecondaryKeyTtl(&g_secondaryUnicastKeyTtlTime, sizeof(g_secondaryUnicastKeyTtlTime));

	checkActiveUnicastKey();

#if SBLCP_DEBUG
	ets_printf("New Secondary uni-cast UDP key %s is stored to NVS at %d\n", DeviceInfo.UnicastSecondaryUDPKey, g_secondaryUnicastKeyStatus.secondary_unicast_key_download_time);
	ets_printf("g_secondaryUnicastKeyTtlTime %d\n", g_secondaryUnicastKeyTtlTime);
#endif
}

//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        update_broadcastPrimaryKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates new primary broadcast key and its status also stores it NVS
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void update_broadcastPrimaryKey()
{
	g_primaryBroadcastKeyStatus.primary_broadcast_key_download_time = GetEpochTime();

#if SBLCP_UDP_KEY_DEBUG
	g_primaryBroadcastKeyStatus.primary_broadcast_key_expiry_time = g_primaryBroadcastKeyStatus.primary_broadcast_key_download_time + TEN_MIN_SECOND_COUNT;
	g_primaryBroadcastKeyTtlTime = UDP_KEY_TTL_MINS;
#else
	g_primaryBroadcastKeyStatus.primary_broadcast_key_expiry_time = g_primaryBroadcastKeyStatus.primary_broadcast_key_download_time + SEVEN_DAYS_SECOND_COUNT;
	g_primaryBroadcastKeyTtlTime = UDP_KEY_TTL_DAYS;
#endif
//	g_broadCastKey_TTL_hour_count = 0;
	g_primaryBroadcastKeyStatus.primary_broadcast_key_expire = false;

	SBLCP_HAL_EERAM_Write_broadcastPrimaryKeyStatus((uint8_t *)&g_primaryBroadcastKeyStatus, sizeof(g_primaryBroadcastKeyStatus));
	SBLCP_HAL_EERAM_Write_BroadcastPrimaryKeyTtl(&g_primaryBroadcastKeyTtlTime, sizeof(g_primaryBroadcastKeyTtlTime));

	checkActiveBroadcastKey();

#if SBLCP_DEBUG
	ets_printf("New Broadcast UDP key %s is stored to NVS at %d\n", DeviceInfo.BroadcastPrimaryUDPKey, g_primaryBroadcastKeyStatus.primary_broadcast_key_download_time);
	ets_printf("g_broadcastKeyTtlTime %d \n", g_primaryBroadcastKeyTtlTime);
	ets_printf("\n");
#endif
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        update_broadcastSecondaryKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function updates new secondary broadcast key and its status also stores it NVS
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void update_broadcastSecondaryKey()
{
	g_secondaryBroadcastKeyStatus.secondary_broadcast_key_download_time = GetEpochTime();

#if SBLCP_UDP_KEY_DEBUG
	g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expiry_time = g_secondaryBroadcastKeyStatus.secondary_broadcast_key_download_time + TEN_MIN_SECOND_COUNT;
	g_secondaryBroadcastKeyTtlTime = UDP_KEY_TTL_MINS;
#else
	g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expiry_time = g_secondaryBroadcastKeyStatus.secondary_broadcast_key_download_time + SEVEN_DAYS_SECOND_COUNT;
	g_secondaryBroadcastKeyTtlTime = UDP_KEY_TTL_DAYS;
#endif

//	g_broadCastKey_TTL_hour_count = 0;
	g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire = false;

	SBLCP_HAL_EERAM_Write_broadcastSecondaryKeyStatus((uint8_t *)&g_secondaryBroadcastKeyStatus, sizeof(g_secondaryBroadcastKeyStatus));

	SBLCP_HAL_EERAM_Write_BroadcastSecondaryKeyTtl(&g_secondaryBroadcastKeyTtlTime, sizeof(g_secondaryBroadcastKeyTtlTime));

	checkActiveBroadcastKey();

#if SBLCP_DEBUG
	ets_printf("New Broadcast UDP key %s is stored to NVS at %d\n", DeviceInfo.BroadcastSecondaryUDPKey, g_secondaryBroadcastKeyStatus.secondary_broadcast_key_download_time);
	ets_printf("g_secondaryBroadcastKeyTtlTime %d \n", g_secondaryBroadcastKeyTtlTime);
	ets_printf("\n");
#endif
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_enbaleFactoyMode()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function enables factory mode
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_enbaleFactoyMode()
{
#if SBLCP_DEBUG
	ets_printf("Enabling Factory Mode\n");
#endif
	//g_SblcpFactoryMode = 0;
	DeviceInfo.DeviceInFactoryMode = DEVICE_IS_IN_FACTORY_MODE;
	WriteByteToFlash(EEOFFSET_SBLCP_FACTORY_MODE,DeviceInfo.DeviceInFactoryMode);

}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        set_FactoryModeStatus()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function set the factory mode status
 *
 * @param[in] 	Factory Mode Enable / Disable
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void set_FactoryModeStatus(uint8_t factory_mode)
{
	//g_SblcpFactoryMode = factory_mode;
	DeviceInfo.DeviceInFactoryMode = factory_mode;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        get_FactoryModeStatus()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function returns the factory mode status
 *
 * @param[in] 	none
 * @retval     	Factory mode Enable / Factory Mode Disable
*/
//--------------------------------------------------------------------------------------------------
uint8_t get_FactoryModeStatus()
{
	//return g_SblcpFactoryMode;
	return DeviceInfo.DeviceInFactoryMode;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        get_factoryModeStatus()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function returns the Factory mode status
 *
 * @param[in] 	none
 * @retval     	true/false (Factory mode disable / Factory mode enable)
*/
//--------------------------------------------------------------------------------------------------
uint8_t get_factoryModeDisableStatus()
{
	uint8_t status;
	//if(g_SblcpFactoryMode == FACTORY_MODE_DISABLE)
	if(DeviceInfo.DeviceInFactoryMode == FACTORY_MODE_DISABLE)
	{
		status = true;
	}
	else
	{
		status = false;
	}
	return status;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        get_manufacturingModeStatus()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function returns the manufacturing mode status
 *
 * @param[in] 	none
 * @retval     	true/false (manufacturing_Mode enable / manufacturing_Mode disable)
*/
//--------------------------------------------------------------------------------------------------
int8_t get_sblcpManufacturingModeOnStatus()
{
	int8_t status;
	if(replyStatusInfo.manufacturing_Mode == MANUFACTURING_MODE_ON)
	{
		status = true;
	}
	else if(replyStatusInfo.manufacturing_Mode == MANUFACTURING_MODE_OFF)
	{
		status = false;
	}
	else
	{
		status = SBLCP_ERROR;
	}
	return status;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        sblcp_updateStatus()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function sets replyStatusInfo.status_updated when data is updated
 *
 * @param[in] 	true / false
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void sblcp_updateReplyMsgStatus(uint8_t status)
{
	if(status)
	{
		replyStatusInfo.status_updated = true;
	}
	else
	{
		replyStatusInfo.status_updated = false;
	}
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        checkPrimaryUniKeyExpiry()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function checks the expirty of primary unicast key
 *
 * @param[in] 	epoc time
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void checkPrimaryUniKeyExpiry(uint32_t key_ttl_time)
{

	g_primaryUnicastKeyTtlTime--;
	if((int)key_ttl_time > 0)
	{
		//Unicast Key expiry time is checked, Both offline count as well as network time is checked for unicast key expiry
		if((g_primaryUnicastKeyTtlTime <= 0) || (key_ttl_time > g_primaryUnicastKeyStatus.primary_unicast_key_expiry_time))
		{
			g_primaryUnicastKeyTtlTime = 0;
			g_primaryUnicastKeyStatus.primary_unicast_key_expire = true;
			g_primaryUnicastKeyStatus.primary_unicast_key_download_time = 0;
			g_primaryUnicastKeyStatus.primary_unicast_key_expiry_time = 0;
			SBLCP_HAL_EERAM_Write_unicastPrimaryKeyStatus((uint8_t *)&g_primaryUnicastKeyStatus, sizeof(g_primaryUnicastKeyStatus));
			delete_primaryUnicastudpKeys();
#if SBLCP_DEBUG
		ets_printf("Primary Unicast key TTL remaining hours  %d \n", g_primaryUnicastKeyTtlTime);
		ets_printf("Primary Unicast UDP Key expires, as 7 Days are over for primary unicast UDP key\n");
#endif
		}
	}
#if SBLCP_DEBUG
			ets_printf("Primary Unicast key TTL remaining hours  %d \n", g_primaryUnicastKeyTtlTime);
#endif
	SBLCP_HAL_EERAM_Write_unicastPrimaryKeyTtl(&g_primaryUnicastKeyTtlTime, sizeof(g_primaryUnicastKeyTtlTime));
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        checkSecondaryUniKeyExpiry()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function checks the expirty of secondary unicast key
 *
 * @param[in] 	epoc time
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void checkSecondaryUniKeyExpiry(uint32_t key_ttl_time)
{
	g_secondaryUnicastKeyTtlTime--;
	if((int)key_ttl_time > 0)
	{
		//Unicast Key expiry time is checked, Both offline count as well as network time is checked for unicast key expiry
		if((g_secondaryUnicastKeyTtlTime <= 0) || (key_ttl_time > g_secondaryUnicastKeyStatus.secondary_unicast_key_expiry_time))
		{
			g_secondaryUnicastKeyTtlTime = 0;
			g_secondaryUnicastKeyStatus.secondary_unicast_key_expire = true;
			g_secondaryUnicastKeyStatus.secondary_unicast_key_download_time = 0;
			g_secondaryUnicastKeyStatus.secondary_unicast_key_expiry_time = 0;
			SBLCP_HAL_EERAM_Write_unicastSecondaryKeyStatus((uint8_t *)&g_secondaryUnicastKeyStatus, sizeof(g_secondaryUnicastKeyStatus));
			delete_secondaryUnicastudpKeys();
#if SBLCP_DEBUG
		ets_printf("Secondary Unicast key TTL remaining hours  %d \n", g_secondaryUnicastKeyTtlTime);
		ets_printf("Secondary Unicast UDP Key expires, as 7 Days are over for secondary unicast UDP key\n");
#endif
		}
	}
#if SBLCP_DEBUG
		ets_printf("Secondary Unicast key TTL remaining hours  %d \n", g_secondaryUnicastKeyTtlTime);
#endif
	SBLCP_HAL_EERAM_Write_unicastSecondaryKeyTtl(&g_secondaryUnicastKeyTtlTime, sizeof(g_secondaryUnicastKeyTtlTime));
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        checkPrimaryBroadKeyExpiry()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function checks the expirty of primary broadcast key
 *
 * @param[in] 	epoc time
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void checkPrimaryBroadKeyExpiry(uint32_t key_ttl_time)
{
	g_primaryBroadcastKeyTtlTime--;
	if((int)key_ttl_time > 0)
	{
		//Broadcast Key expiry time is checked, Both offline count as well as network time is checked for Broadcast key expiry
		if((g_primaryBroadcastKeyTtlTime <= 0) || (key_ttl_time > g_primaryBroadcastKeyStatus.primary_broadcast_key_expiry_time))
		{
			g_primaryBroadcastKeyTtlTime = 0;
			g_primaryBroadcastKeyStatus.primary_broadcast_key_expire = true;
			g_primaryBroadcastKeyStatus.primary_broadcast_key_download_time = 0;
			g_primaryBroadcastKeyStatus.primary_broadcast_key_expiry_time = 0;
			SBLCP_HAL_EERAM_Write_broadcastPrimaryKeyStatus((uint8_t *)&g_primaryBroadcastKeyStatus, sizeof(g_primaryBroadcastKeyStatus));
			delete_primary_BroadcastKey();
#if SBLCP_DEBUG
			ets_printf("Primary Broadcast key TTL remaining hours %d\n", g_primaryBroadcastKeyTtlTime);
			ets_printf("Primary Broadcast UDP Key expires, as 7 Days are over for Broadcast UDP key\n");
#endif
		}
	}
#if SBLCP_DEBUG
		ets_printf("Primary Broadcast key TTL remaining hours %d\n", g_primaryBroadcastKeyTtlTime);
#endif
	SBLCP_HAL_EERAM_Write_BroadcastPrimaryKeyTtl(&g_primaryBroadcastKeyTtlTime, sizeof(g_primaryBroadcastKeyTtlTime));
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        checkSecondaryBroadKeyExpiry()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function checks the expirty of secondary broadcast key
 *
 * @param[in] 	epoc time
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void checkSecondaryBroadKeyExpiry(uint32_t key_ttl_time)
{
	g_secondaryBroadcastKeyTtlTime--;
	if((int)key_ttl_time > 0)
	{
		//Broadcast Key expiry time is checked, Both offline count as well as network time is checked for Broadcast key expiry
		if((g_secondaryBroadcastKeyTtlTime <= 0) || (key_ttl_time > g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expiry_time))
		{
			g_secondaryBroadcastKeyTtlTime = 0;
			g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire = true;
			g_secondaryBroadcastKeyStatus.secondary_broadcast_key_download_time = 0;
			g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expiry_time = 0;
			SBLCP_HAL_EERAM_Write_broadcastSecondaryKeyStatus((uint8_t *)&g_secondaryBroadcastKeyStatus, sizeof(g_secondaryBroadcastKeyStatus));
			delete_secondary_BroadcastKey();
#if SBLCP_DEBUG
			ets_printf("secondary Broadcast key TTL remaining hours %d\n", g_secondaryBroadcastKeyTtlTime);
			ets_printf("secondary Broadcast UDP Key expires, as 7 Days are over for secondary Broadcast UDP key\n");
#endif
		}
	}
#if SBLCP_DEBUG
		ets_printf("Secondary Broadcast key TTL remaining hours %d\n", g_secondaryBroadcastKeyTtlTime);
#endif
	SBLCP_HAL_EERAM_Write_BroadcastSecondaryKeyTtl(&g_secondaryBroadcastKeyTtlTime, sizeof(g_secondaryBroadcastKeyTtlTime));
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        reset_primaryUnicastkey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function resets primary unicast key
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t reset_primaryUnicastkey()
{

#if SBLCP_DEBUG
	ets_printf("Resetting Primary unicast keys\n");
#endif

	eeram_err_t ret = EERAM_SUCCESS;
	EERAM_Message_Packet status_Packet;

	g_primaryUnicastKeyStatus.primary_unicast_key_expire = true;
	g_primaryUnicastKeyStatus.primary_unicast_key_download_time = 0;
	g_primaryUnicastKeyStatus.primary_unicast_key_expiry_time = 0;

//	SBLCP_HAL_EERAM_Write_unicastPrimaryKeyStatus((uint8_t *)&g_primaryUnicastKeyStatus, sizeof(g_primaryUnicastKeyStatus));
	EERAM_Create_Message_Packet(&status_Packet, &g_primaryUnicastKeyStatus, EERAM_UNICAST_PRIMARY_UDP_KEY_STATUS_ADD, sizeof(g_primaryUnicastKeyStatus));
	if(EERAM_Get_Task_Handle() == NULL)
	{
		if (EERAM_Write_data(&status_Packet) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}
	else
	{
		EERAM_Send_Message_Packet(&status_Packet);
	}

	EERAM_Message_Packet ttl_data;
	g_primaryUnicastKeyTtlTime = 0;
//	SBLCP_HAL_EERAM_Write_unicastPrimaryKeyTtl(&g_primaryUnicastKeyTtlTime, sizeof(g_primaryUnicastKeyTtlTime));
	EERAM_Create_Message_Packet(&ttl_data, &g_primaryUnicastKeyTtlTime, EERAM_UNICAST_PRIMARY_KEY_TTL_ADD, sizeof(g_primaryUnicastKeyTtlTime));
	if(EERAM_Get_Task_Handle() == NULL)
	{
		if (EERAM_Write_data(&ttl_data) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}
	else
	{
		EERAM_Send_Message_Packet(&ttl_data);
	}

	if(EERAM_Get_Task_Handle() != NULL)
	{
		delete_primaryUnicastudpKeys();
	}
	checkActiveUnicastKey();
//	delete_primaryUnicastudpKeys();
	return ret;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        reset_secondaryUnicastkey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function resets secondary unicast key
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t reset_secondaryUnicastkey()
{
#if SBLCP_DEBUG
	ets_printf("Resetting Secondary unicast keys\n");
#endif

	eeram_err_t ret = EERAM_SUCCESS;
	EERAM_Message_Packet status_Packet;

	g_secondaryUnicastKeyStatus.secondary_unicast_key_expire = true;
	g_secondaryUnicastKeyStatus.secondary_unicast_key_download_time = 0;
	g_secondaryUnicastKeyStatus.secondary_unicast_key_expiry_time = 0;
//	SBLCP_HAL_EERAM_Write_unicastSecondaryKeyStatus((uint8_t *)&g_secondaryUnicastKeyStatus, sizeof(g_secondaryUnicastKeyStatus));
	EERAM_Create_Message_Packet(&status_Packet, &g_secondaryUnicastKeyStatus, EERAM_UNICAST_SECONDARY_UDP_KEY_STATUS_ADD, sizeof(g_secondaryUnicastKeyStatus));
	if(EERAM_Get_Task_Handle() == NULL)
	{
		if(EERAM_Write_data(&status_Packet) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}
	else
	{
		EERAM_Send_Message_Packet(&status_Packet);
	}

	EERAM_Message_Packet ttl_data;
	g_secondaryUnicastKeyTtlTime = 0;
//	SBLCP_HAL_EERAM_Write_unicastSecondaryKeyTtl(&g_secondaryUnicastKeyTtlTime, sizeof(g_secondaryUnicastKeyTtlTime));
	EERAM_Create_Message_Packet(&ttl_data, &g_secondaryUnicastKeyTtlTime, EERAM_UNICAST_SECONDARY_KEY_TTL_ADD, sizeof(g_secondaryUnicastKeyTtlTime));
	if(EERAM_Get_Task_Handle() == NULL)
	{
		if (EERAM_Write_data(&ttl_data) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}
	else
	{
		EERAM_Send_Message_Packet(&ttl_data);
	}
	if(EERAM_Get_Task_Handle() != NULL)
	{
		delete_secondaryUnicastudpKeys();
	}
	checkActiveUnicastKey();
//	delete_secondaryUnicastudpKeys();
	return ret;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        reset_primaryBroadcastkey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function resets primary Broadcast key
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t reset_primaryBroadcastkey()
{
#if SBLCP_DEBUG
	ets_printf("Resetting Primary Broadcast keys\n");
#endif

	eeram_err_t ret = EERAM_SUCCESS;
	EERAM_Message_Packet status_Packet;
	g_primaryBroadcastKeyStatus.primary_broadcast_key_expire = true;
	g_primaryBroadcastKeyStatus.primary_broadcast_key_download_time = 0;
	g_primaryBroadcastKeyStatus.primary_broadcast_key_expiry_time = 0;
//	SBLCP_HAL_EERAM_Write_broadcastPrimaryKeyStatus((uint8_t *)&g_primaryBroadcastKeyStatus, sizeof(g_primaryBroadcastKeyStatus));
	EERAM_Create_Message_Packet(&status_Packet, &g_primaryBroadcastKeyStatus, EERAM_BROADCAST_PRIMARY_UDP_KEY_STAUS_ADD, sizeof(g_primaryBroadcastKeyStatus));
	if(EERAM_Get_Task_Handle() == NULL)
	{
		if(EERAM_Write_data(&status_Packet) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}
	else
	{
		EERAM_Send_Message_Packet(&status_Packet);
	}

	EERAM_Message_Packet ttl_data;
	g_primaryBroadcastKeyTtlTime = 0;
//	SBLCP_HAL_EERAM_Write_BroadcastPrimaryKeyTtl(&g_primaryBroadcastKeyTtlTime, sizeof(g_primaryBroadcastKeyTtlTime));
	EERAM_Create_Message_Packet(&ttl_data, &g_primaryBroadcastKeyTtlTime, EERAM_BROADCAST_PRIMARY_KEY_TTL_ADD, sizeof(g_primaryBroadcastKeyTtlTime));
	if(EERAM_Get_Task_Handle() == NULL)
	{
		if (EERAM_Write_data(&ttl_data) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}
	else
	{
		EERAM_Send_Message_Packet(&ttl_data);
	}
	if(EERAM_Get_Task_Handle() != NULL)
	{
		delete_primary_BroadcastKey();
	}
	checkActiveBroadcastKey();
//	delete_primary_BroadcastKey();
	return ret;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        reset_secondaryBroadcastkey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function resets secondary Broadcast key
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t reset_secondaryBroadcastkey()
{
#if SBLCP_DEBUG
	ets_printf("Resetting Secondary Broadcast keys\n");
#endif

	eeram_err_t ret = EERAM_SUCCESS;
	EERAM_Message_Packet status_Packet;

	g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire = true;
	g_secondaryBroadcastKeyStatus.secondary_broadcast_key_download_time = 0;
	g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expiry_time = 0;
//	SBLCP_HAL_EERAM_Write_broadcastSecondaryKeyStatus((uint8_t *)&g_secondaryBroadcastKeyStatus, sizeof(g_secondaryBroadcastKeyStatus));
	EERAM_Create_Message_Packet(&status_Packet, &g_secondaryBroadcastKeyStatus, EERAM_BROADCAST_SECONDARY_UDP_KEY_STAUS_ADD, sizeof(g_secondaryBroadcastKeyStatus));
	if (EERAM_Write_data(&status_Packet) != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR; //I2C ERROR
	}

	EERAM_Message_Packet ttl_data;
	g_secondaryBroadcastKeyTtlTime = 0;
//	SBLCP_HAL_EERAM_Write_BroadcastSecondaryKeyTtl(&g_secondaryBroadcastKeyTtlTime, sizeof(g_secondaryBroadcastKeyTtlTime));
	EERAM_Create_Message_Packet(&ttl_data, &g_secondaryBroadcastKeyTtlTime, EERAM_BROADCAST_SECONDARY_KEY_TTL_ADD, sizeof(g_secondaryBroadcastKeyTtlTime));
	if (EERAM_Write_data(&ttl_data) != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR; //I2C ERROR
	}

	if(EERAM_Get_Task_Handle() != NULL)
	{
		delete_secondary_BroadcastKey();
	}
	checkActiveBroadcastKey();
//	delete_secondary_BroadcastKey();

	return ret;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        checkUdpKeyExpiry()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function check the validity all UDP keys every hour
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void checkUdpKeyExpiry()
{
	if(!g_primaryUnicastKeyStatus.primary_unicast_key_expire)
	{
		checkPrimaryUniKeyExpiry(GetEpochTime());
	}
	else
	{
#if SBLCP_DEBUG
//		ets_printf("Primary Unicast key Expired \n");
#endif
	}
	if(!g_primaryBroadcastKeyStatus.primary_broadcast_key_expire)
	{
		checkPrimaryBroadKeyExpiry(GetEpochTime());
	}
	else
	{
#if SBLCP_DEBUG
//		ets_printf("Primary Broadcast key Expired \n");
#endif
	}

	if(!g_secondaryUnicastKeyStatus.secondary_unicast_key_expire)
	{
		checkSecondaryUniKeyExpiry(GetEpochTime());
	}
	else
	{
#if SBLCP_DEBUG
//		ets_printf("secondary Unicast key Expired \n");
#endif
	}
	if(!g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire)
	{
		checkSecondaryBroadKeyExpiry(GetEpochTime());
	}
	else
	{
#if SBLCP_DEBUG
//		ets_printf("Primary Broadcast key Expired \n");
#endif
	}
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        checkActiveUnicastKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function check for active unicast key every hour to be use for hash calculation
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void checkActiveUnicastKey()
{
	activeUDPkey_en active_key = NO_UDP_KEY;
	if((g_primaryUnicastKeyStatus.primary_unicast_key_expire) && (g_secondaryUnicastKeyStatus.secondary_unicast_key_expire))
	{
		active_key = NO_UDP_KEY;
	}
	else if((!g_primaryUnicastKeyStatus.primary_unicast_key_expire) && (!g_secondaryUnicastKeyStatus.secondary_unicast_key_expire))
	{

		if(g_primaryUnicastKeyTtlTime == g_secondaryUnicastKeyTtlTime)
		{
			active_key = PRIMARY_UNICAST;
		}
		else if(g_primaryUnicastKeyTtlTime < g_secondaryUnicastKeyTtlTime)
		{
			active_key = PRIMARY_UNICAST;
		}
		else
		{
			active_key = SECONDARY_UNICAST;
		}
	}
	else if(!g_primaryUnicastKeyStatus.primary_unicast_key_expire)
	{
		active_key = PRIMARY_UNICAST;
	}
	else
	{
		active_key = SECONDARY_UNICAST;
	}
#if SBLCP_DEBUG
	ets_printf("Active Unicast Key %d \n", active_key);
#endif
	setActiveUnicastKey(active_key);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        checkActiveBroadcastKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function check for active broadcast key every hour to be use for hash calculation
 *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void checkActiveBroadcastKey()
{
	activeUDPkey_en active_key = NO_UDP_KEY;
	if((g_primaryBroadcastKeyStatus.primary_broadcast_key_expire) && (g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire))
	{
		active_key = NO_UDP_KEY;
	}
	else if((!g_primaryBroadcastKeyStatus.primary_broadcast_key_expire) && (!g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire))
	{
		if(g_primaryBroadcastKeyTtlTime == g_secondaryBroadcastKeyTtlTime)
		{
			active_key = PRIMARY_BROADCAST;
		}
		else if(g_primaryBroadcastKeyTtlTime < g_secondaryBroadcastKeyTtlTime)
		{
			active_key = PRIMARY_BROADCAST;
		}
		else
		{
			active_key = SECONDARY_BROADCAST;
		}
	}
	else if(!g_primaryBroadcastKeyStatus.primary_broadcast_key_expire)
	{
		active_key = PRIMARY_BROADCAST;
	}
	else
	{
		active_key = SECONDARY_BROADCAST;
	}
#if SBLCP_DEBUG
	ets_printf("Active broadcast Key %d \n", active_key);
#endif
	setActiveBroadcastKey(active_key);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        setActiveUnicastKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function sets the status of active unicast key
 *
 * @param[in] 	active key
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void setActiveUnicastKey(activeUDPkey_en active_key)
{
	g_unicastActiveKey = active_key;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        getActiveUnicastKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function returns the status of active unicast key
 *
 * @param[in] 	none
 * @retval     	active key status
*/
//--------------------------------------------------------------------------------------------------
activeUDPkey_en getActiveUnicastKey()
{
	return g_unicastActiveKey;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        setActiveBroadcastKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function sets the status of active Broadcast key
 *
 * @param[in] 	active key
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void setActiveBroadcastKey(activeUDPkey_en active_key)
{
	g_broadcastActiveKey =  active_key;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        hexToString()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function converts the integer data into string for calculation of
 * 				crypto hash
 *
 * @param[in]   1. destination buffer address 2. source buffer address 3. source buffer length
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void hexToString(unsigned char *outbuff, uint8_t *srcBuff, uint16_t srcBuffLen)
{
	//to avoid termination due to detection of 0 as NULL,
	//below loop checks for any '0' in message, if appears convert it to ascii
	char charData[5];
	uint16_t index = 0;
	for(uint32_t i = 0; i < srcBuffLen; i++)
	{
		sprintf(charData, "%d", srcBuff[i]);
		int j = 0;
		while(charData[j] != NULL)
		{
			outbuff[index] = charData[j];
			j++;
			index++;
		}
	}
	outbuff[index] = NULL;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        getActiveBroadcastKey()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function returns the status of active broadcast key
 *
 * @param[in] 	none
 * @retval     	active key status
*/
//--------------------------------------------------------------------------------------------------
activeUDPkey_en getActiveBroadcastKey()
{
	return g_broadcastActiveKey;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        setMesaageType()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		function sets the received message type to broadcast or unicast message
 * 				and sets the type of message
 *
 * @param[in] 	broadcast message/unicast message
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void setMesaageType(uint8_t msg_type)
{
	g_messageType = msg_type;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        getMesaageType()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		function returns the message type as broadcast or unicast message
 * 				and sets the type of message
 *
 * @param[in] 	none
 * @retval     	broadcast message/unicast message
*/
//--------------------------------------------------------------------------------------------------
uint8_t getMesaageType()
{
	return g_messageType;
}
int8_t get_UdpKeyForHashCalculation(unsigned char *udp_key, uint8_t keyType)
{
	int8_t iRet = SBLCP_SUCCESS;
	//if(g_SblcpFactoryMode == FACTORY_MODE_DISABLE)
	if(DeviceInfo.DeviceInFactoryMode == FACTORY_MODE_DISABLE)
	{

		if((g_primaryBroadcastKeyStatus.primary_broadcast_key_expire) && (g_primaryUnicastKeyStatus.primary_unicast_key_expire) &&
				(g_secondaryBroadcastKeyStatus.secondary_broadcast_key_expire) && (g_secondaryUnicastKeyStatus.secondary_unicast_key_expire))
		{
			ets_printf("Both Unicast and Broadcast UDP keys are expired, please update both keys\n");
			iRet = SBLCP_ERROR;
			return iRet;
		}
		else
		{
			if(keyType == BORADCAST_KEY)
			{
				activeUDPkey_en broadcast_key = getActiveBroadcastKey();
				if(broadcast_key == NO_UDP_KEY)
				{
					iRet = SBLCP_ERROR;
					return iRet;
				}
				else
				{
					if(broadcast_key == PRIMARY_BROADCAST)
					{
						memcpy(udp_key, DeviceInfo.BroadcastPrimaryUDPKey, MAX_HASHLEN);
					}
					else
					{
						memcpy(udp_key, DeviceInfo.BroadcastSecondaryUDPKey, MAX_HASHLEN);
					}
				}
			}
			else if(keyType == UNICAST_KEY)
			{
				activeUDPkey_en unicast_key = getActiveUnicastKey();
				if(unicast_key == NO_UDP_KEY)
				{
					iRet = SBLCP_ERROR;
					return iRet;
				}
				else
				{
					if(unicast_key == PRIMARY_UNICAST)
					{
						memcpy(udp_key, DeviceInfo.UnicastPrimaryUDPKey, MAX_HASHLEN);
					}
					else
					{
						memcpy(udp_key, DeviceInfo.UnicastSecondaryUDPKey, MAX_HASHLEN);
					}
				}
			}
			else
			{
				iRet = SBLCP_ERROR;
				return iRet;
			}
		}
	}//factory disable
	else
	{
#if SBLCP_DEBUG
		ets_printf("Factory Mode is Enable, Use Default key\n");
#endif
		const unsigned char fix_key_for_factory[CRYPTO_HASH_LENGTH] = "ba32b432c6e911edafa10242ac120002";
		memcpy(udp_key, fix_key_for_factory, MAX_HASHLEN);
	}
	return iRet;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_Write_unicastPrimaryKeyTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function writes primary unicast TTL
*
*
* @param[in]   uint32_t pointer primary unicast ttl, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_HAL_EERAM_Write_unicastPrimaryKeyTtl(uint32_t *uniCastPrimaryTtl, size_t length)
{
	 EERAM_Message_Packet eeram_message_packet;

	 EERAM_Create_Message_Packet(&eeram_message_packet, uniCastPrimaryTtl, EERAM_UNICAST_PRIMARY_KEY_TTL_ADD, length);
	 EERAM_Send_Message_Packet(&eeram_message_packet);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_Write_unicastPrimaryKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function writes primary Unicast udp key status
*
*
* @param[in]  uint8_t pointer to primary unicast key status, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_HAL_EERAM_Write_unicastPrimaryKeyStatus(uint8_t *unicastPrimaryStatus, size_t length)
{
	EERAM_Message_Packet eeram_message_packet;

	EERAM_Create_Message_Packet(&eeram_message_packet, unicastPrimaryStatus, EERAM_UNICAST_PRIMARY_UDP_KEY_STATUS_ADD, length);
	EERAM_Send_Message_Packet(&eeram_message_packet);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_ReadPrimaryUnicastTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function read primary unicast TTL
*
*
* @param[in]  uint32_t primary unicast ttl, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_HAL_EERAM_ReadPrimaryUnicastTtl(uint32_t *uniCastPrimaryTtl, size_t length)
{
	return EERAM_Read_data(EERAM_UNICAST_PRIMARY_KEY_TTL_ADD, uniCastPrimaryTtl, length);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_ReadSecondaryUnicastTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief	This function read secondary unicast TTL
*
* @param[in] uint32_t pointer to secondary unicast ttl, size_t length
* @retval    EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_HAL_EERAM_ReadSecondaryUnicastTtl(uint32_t *uniCastsecondaryTtl, size_t length)
{
	return EERAM_Read_data(EERAM_UNICAST_SECONDARY_KEY_TTL_ADD, uniCastsecondaryTtl, length);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_ReadPrimaryBroadcastTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief	This function read Primary Broadcast TTL
*
* @param[in] uint32_t pointer to Primary Broadcast ttl, size_t length
* @retval    EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_HAL_EERAM_ReadPrimaryBroadcastTtl(uint32_t *BroadcastPrimaryTtl, size_t length)
{
	return EERAM_Read_data(EERAM_BROADCAST_PRIMARY_KEY_TTL_ADD, BroadcastPrimaryTtl, length);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_ReadSecondaryBroadcastTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function read secondary broadcast TTL
*
*
* @param[in] 	uint32_t secondary broadcast ttl, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_HAL_EERAM_ReadSecondaryBroadcastTtl(uint32_t *BroadcastSecondaryTtl, size_t length)
{
	return EERAM_Read_data(EERAM_BROADCAST_SECONDARY_KEY_TTL_ADD, BroadcastSecondaryTtl, length);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_Write_unicastSecondaryKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function writes Secondary Unicast udp key status
*
*
* @param[in] 	uint8_t pointer to secondary unicast key status, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_HAL_EERAM_Write_unicastSecondaryKeyStatus(uint8_t *unicastSecondaryStatus, size_t length)
{
	 EERAM_Message_Packet eeram_message_packet;

	 EERAM_Create_Message_Packet(&eeram_message_packet, unicastSecondaryStatus, EERAM_UNICAST_SECONDARY_UDP_KEY_STATUS_ADD, length);
	 EERAM_Send_Message_Packet(&eeram_message_packet);
}

//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_Write_broadcastPrimaryKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function writes primary broadcast udp key status
*
*
* @param[in] 	uint8_t pointer to primary broadcast key status, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_HAL_EERAM_Write_broadcastPrimaryKeyStatus(uint8_t *broadcastPrimaryStatus, size_t length)
{
	 EERAM_Message_Packet eeram_message_packet;

	 EERAM_Create_Message_Packet(&eeram_message_packet, broadcastPrimaryStatus, EERAM_BROADCAST_PRIMARY_UDP_KEY_STAUS_ADD, length);
	 EERAM_Send_Message_Packet(&eeram_message_packet);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_Write_unicastSecondaryKeyTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function writes secondary unicast TTL
*
*
* @param[in] 	uint32_t unicast ttl, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_HAL_EERAM_Write_unicastSecondaryKeyTtl(uint32_t *unicastSecondaryTtl, size_t length)
{
	EERAM_Message_Packet eeram_message_packet;

	EERAM_Create_Message_Packet(&eeram_message_packet, unicastSecondaryTtl, EERAM_UNICAST_SECONDARY_KEY_TTL_ADD, length);
	EERAM_Send_Message_Packet(&eeram_message_packet);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_Write_BroadcastPrimaryKeyTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function writes primary broadcast TTL
*
*
* @param[in] 	uint32_t primary broadcast ttl, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_HAL_EERAM_Write_BroadcastPrimaryKeyTtl(uint32_t *BroadCastPrimaryTtl, size_t length)
{
	 EERAM_Message_Packet eeram_message_packet;

	 EERAM_Create_Message_Packet(&eeram_message_packet, BroadCastPrimaryTtl, EERAM_BROADCAST_PRIMARY_KEY_TTL_ADD, length);
	 EERAM_Send_Message_Packet(&eeram_message_packet);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_Write_broadcastSecondaryKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function writes secondary broadcast udp key status
*
*
* @param[in] 	uint8_t pointer to secondary broadcast key status, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_HAL_EERAM_Write_broadcastSecondaryKeyStatus(uint8_t *broadcastSecondaryStatus, size_t length)
{
	 EERAM_Message_Packet eeram_message_packet;

	 EERAM_Create_Message_Packet(&eeram_message_packet, broadcastSecondaryStatus, EERAM_BROADCAST_SECONDARY_UDP_KEY_STAUS_ADD, length);
	 EERAM_Send_Message_Packet(&eeram_message_packet);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_Write_BroadcastSecondaryKeyTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function writes secondary broadcast TTL
*
*
* @param[in] 	uint32_t secondary broadcast ttl, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_HAL_EERAM_Write_BroadcastSecondaryKeyTtl(uint32_t *BroadcastSecondaryTtl, size_t length)
{
	EERAM_Message_Packet eeram_message_packet;

	EERAM_Create_Message_Packet(&eeram_message_packet, BroadcastSecondaryTtl, EERAM_BROADCAST_SECONDARY_KEY_TTL_ADD, length);
	EERAM_Send_Message_Packet(&eeram_message_packet);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_ReadUnicastPrimaryKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function reads primary unicast udp key status
*
*
* @param[in] 	uint8_t pointer to primary unicast key status, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_HAL_EERAM_ReadUnicastPrimaryKeyStatus(uint8_t *uniCastprimaryStatus, size_t length)
{
	return EERAM_Read_data(EERAM_UNICAST_PRIMARY_UDP_KEY_STATUS_ADD, uniCastprimaryStatus, length);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_ReadUnicastSecondaryKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function reads secondary unicast udp key status
*
*
* @param[in] 	uint8_t pointer to secondary unicast key status, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_HAL_EERAM_ReadUnicastSecondaryKeyStatus(uint8_t *unicastsecondaryStatus, size_t length)
{
	return EERAM_Read_data(EERAM_UNICAST_SECONDARY_UDP_KEY_STATUS_ADD, unicastsecondaryStatus, length);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_ReadBroadcastPrimaryKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function reads primary broadcast udp key status
*
*
* @param[in] 	uint8_t pointer to primary broadcast key status, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_HAL_EERAM_ReadBroadcastPrimaryKeyStatus(uint8_t *broadCastStatus, size_t length)
{
	return EERAM_Read_data(EERAM_BROADCAST_PRIMARY_UDP_KEY_STAUS_ADD, broadCastStatus, length);
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_HAL_EERAM_ReadBroadcastSecondaryKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		This function reads secondary broadcast udp key status
*
*
* @param[in] 	uint8_t secondary broadcast key status, size_t length
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_HAL_EERAM_ReadBroadcastSecondaryKeyStatus(uint8_t *broadCastsecondaryStatus, size_t length)
{
	return EERAM_Read_data(EERAM_BROADCAST_SECONDARY_UDP_KEY_STAUS_ADD, broadCastsecondaryStatus, length);
}


///**
// * @brief      This function reads factory mode status
// * @param[in]   uint32_t Factory mode status, size_t length
// * @retval     uint8_t err type
// */
//
//uint8_t SBLCP_HAL_EERAM_ReadFactoryMode(uint8_t *factoy_mode, size_t length)
//{
//// 	return EERAM_Read_data(EERAM_SBLCP_FACTORY_MODE_ADD, factoy_mode, length);
//
//	 return EERAM_SUCCESS; //remove
//}
//
//
///**
// * @brief      This function writes factory mode status
// * @param[in]   uint32_t Factory mode status, size_t length
// * @retval     uint8_t err type
// */
//uint8_t SBLCP_HAL_EERAM_Write_FactoryMode(uint8_t *factory_mode, size_t length)
//{
//	 //Aarti: Rahul need to check code lines
//
////	EERAM_Message_Packet eeram_message_packet;
////
////	//Create Message packet
////
////	EERAM_Create_Message_Packet(&eeram_message_packet, factory_mode, EERAM_SBLCP_FACTORY_MODE_ADD, length);
////
////	//Send Packet
////
////	EERAM_Send_Message_Packet(&eeram_message_packet);
//
//	 return EERAM_SUCCESS; //remove
//}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_readUdpKeyStatus()
//--------------------------------------------------------------------------------------------------
/**
* @brief		Function reads all UDP keys status at initialization
*
*
* @param[in] 	none
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_readUdpKeyStatus()
{
	 eeram_err_t ret = EERAM_SUCCESS;

	if(SBLCP_HAL_EERAM_ReadUnicastPrimaryKeyStatus((uint8_t *)&g_primaryUnicastKeyStatus, sizeof(g_primaryUnicastKeyStatus)) == EERAM_SUCCESS)
	{
		if(SBLCP_HAL_EERAM_ReadBroadcastPrimaryKeyStatus((uint8_t *)&g_primaryBroadcastKeyStatus, sizeof(g_primaryBroadcastKeyStatus)) == EERAM_SUCCESS)
		{
			if(SBLCP_HAL_EERAM_ReadUnicastSecondaryKeyStatus((uint8_t *)&g_secondaryUnicastKeyStatus, sizeof(g_secondaryUnicastKeyStatus)) == EERAM_SUCCESS)
			{
				if(SBLCP_HAL_EERAM_ReadBroadcastSecondaryKeyStatus((uint8_t *)&g_secondaryBroadcastKeyStatus, sizeof(g_secondaryBroadcastKeyStatus)) != EERAM_SUCCESS)
				{
					ret = EERAM_I2C_ERR;
				}
			}
			else
			{
				ret = EERAM_I2C_ERR;
			}
		}
		else
		{
			ret = EERAM_I2C_ERR;
		}
	}
	else
	{
		ret = EERAM_I2C_ERR;
	}
#if SBLCP_DEBUG
	ets_printf("UDP key status read done \n");
#endif
	return ret;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_readKeyTtl()
//--------------------------------------------------------------------------------------------------
/**
* @brief		Function reads all UDP key TTL at initialization
*
*
* @param[in] 	none
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t SBLCP_readKeyTtl()
{
	eeram_err_t ret = EERAM_SUCCESS;

	if(SBLCP_HAL_EERAM_ReadPrimaryUnicastTtl(&g_primaryUnicastKeyTtlTime, sizeof(g_primaryUnicastKeyTtlTime)) == EERAM_SUCCESS)
	{
		if(SBLCP_HAL_EERAM_ReadPrimaryBroadcastTtl(&g_primaryBroadcastKeyTtlTime, sizeof(g_primaryBroadcastKeyTtlTime)) == EERAM_SUCCESS)
		{
			if(SBLCP_HAL_EERAM_ReadSecondaryUnicastTtl(&g_secondaryUnicastKeyTtlTime, sizeof(g_secondaryUnicastKeyTtlTime)) == EERAM_SUCCESS)
			{
				if(SBLCP_HAL_EERAM_ReadSecondaryBroadcastTtl(&g_secondaryBroadcastKeyTtlTime, sizeof(g_secondaryBroadcastKeyTtlTime)) != EERAM_SUCCESS)
				{
					ret = EERAM_I2C_ERR;
				}
			}
			else
			{
				ret = EERAM_I2C_ERR;
			}
		}
		else
		{
			ret = EERAM_I2C_ERR;
		}
	}
	else
	{
		ret = EERAM_I2C_ERR;
	}
#if SBLCP_DEBUG
	ets_printf("UDP key TTL read done \n");
#endif
	return ret;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        EERAM_Read_UDP_Keys()
//--------------------------------------------------------------------------------------------------
/**
* @brief		Function reads all UDP keys at initialization
*
*
* @param[in] 	none
* @retval     EERAM_SUCCESS /  EERAM_I2C_ERR
*/
//--------------------------------------------------------------------------------------------------
eeram_err_t EERAM_Read_UDP_Keys()
{
	 eeram_err_t ret = EERAM_SUCCESS;

	 if(SBLCP_readUdpKeyStatus() == EERAM_SUCCESS)
	 {
		 if(SBLCP_readKeyTtl() != EERAM_SUCCESS)
		 {
			 ret = EERAM_I2C_ERR;
		 }
	 }
	 else
	 {
		 ret = EERAM_I2C_ERR;
	 }
	 return ret;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        resetProdDetails()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		function resets all the production details to zero value at very first power on
 *  *
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_resetProdDetails()
{
	DeviceDetails_st prod_details;
	Pcbdetsils_st pcb_details;
	size_t length;

#if SBLCP_DEBUG
	ets_printf("Resetting Production details\n");
#endif

	memset(&prod_details, 0, sizeof(DeviceDetails_st));
	//default current rating is set to 60 Amps
	prod_details.current_rating = MAX_CURRENT_RATING;
	DeviceInfo.current_rating = prod_details.current_rating;
	WriteDataToFlash(EEOFFSET_SET_PRODUCTION_DETAILS, (char *)&prod_details, sizeof(prod_details));

	//reset PCB details
	memset(&pcb_details, 0, sizeof(Pcbdetsils_st));
	WriteDataToFlash(EEOFFSET_SET_PCB_DETAILS, (char *)&pcb_details, sizeof(pcb_details));
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        updateProductionDetails()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		function updates the Device details received to Device info structure
 * 				and saves device info details into ESP NVS
 *  *
 * @param[in] 	pointer to received data
 * @retval     	Ack / Nack
*/
//--------------------------------------------------------------------------------------------------
responceAckNac_e updateProductionDetails(uint8_t *prx_buffer)
{
	DeviceDetails_st prod_details;
	responceAckNac_e ack_nack;

	memset(&prod_details, 0, sizeof(DeviceDetails_st));
	memcpy(&prod_details, prx_buffer + sizeof(sblcpHeader_st), sizeof(DeviceDetails_st));

	//check for the range of current rating
	//Minimum current rating of SB2.0 is 15 Amps and Maximum current rating is 60 Amps
	if((prod_details.current_rating >= MIN_CURRENT_RATING) && (prod_details.current_rating <= MAX_CURRENT_RATING))
	{
		ack_nack = ACK;
	}
	else
	{
		//if requested current rating is less than minimum rating, set current rating to minimum value
		//if requested current rating is greater than maximum rating, set current rating to maximum value
		//if current rating out of this range or unknown value is received, set rating to minimum value

		if(prod_details.current_rating <= MIN_CURRENT_RATING)
		{
			prod_details.current_rating = MIN_CURRENT_RATING;
			ack_nack = ACK;
		}
		else if(prod_details.current_rating >= MAX_CURRENT_RATING)
		{
			prod_details.current_rating = MAX_CURRENT_RATING;
			ack_nack = ACK;
		}
		else
		{
			prod_details.current_rating = MAX_CURRENT_RATING;
			ack_nack = NACK;
		}

#if SBLCP_DEBUG
		ets_printf("Out of range Current value,  currnet_reating set to %d\n", prod_details.current_rating);
#endif
	}
	WriteDataToFlash(EEOFFSET_SET_PRODUCTION_DETAILS, (char *)&prod_details, sizeof(prod_details));

	//update data production details in to Device info structure
	DeviceInfo.current_rating = prod_details.current_rating;
	DeviceInfo.breaker_test_status = prod_details.breaker_test_status;
	memcpy(&DeviceInfo.catalog_number, prod_details.catalog_number, sizeof(prod_details.catalog_number));
	memcpy(&DeviceInfo.style_number, prod_details.style_number, sizeof(prod_details.style_number));
	memcpy(&DeviceInfo.breaker_serial_no, prod_details.breaker_serial_no, sizeof(prod_details.breaker_serial_no));

#if SBLCP_DEBUG
	ets_printf("Current Rating:-[%d]\n", prod_details.current_rating);
	ets_printf("breaker_test_status:-[%d]\n", prod_details.breaker_test_status);
	ets_printf("catalog_number:-[%s]\n", DeviceInfo.catalog_number);
	ets_printf("style_number:-[%s]\n", DeviceInfo.style_number);
	ets_printf("breaker_serial_no:-[%s]\n",DeviceInfo.breaker_serial_no);
#endif

	return ack_nack;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        getProductionDetails()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		function copies the production details of device to send it on request received
 *
 * @param[in] 	pointer prod details structure
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void getProductionDetails(DeviceProductionDetails_st *prod_details)
{
	prod_details->device_details.current_rating = DeviceInfo.current_rating;
	prod_details->device_details.breaker_test_status = DeviceInfo.breaker_test_status;

	memcpy(prod_details->device_details.catalog_number, DeviceInfo.catalog_number, (sizeof(DeviceInfo.catalog_number) - 1));
	memcpy(prod_details->device_details.style_number, DeviceInfo.style_number, (sizeof(DeviceInfo.style_number) - 1));
	memcpy(prod_details->device_details.breaker_serial_no, DeviceInfo.breaker_serial_no, (sizeof(DeviceInfo.breaker_serial_no) - 1));

	memcpy(prod_details->ESP_firmware_ver, DeviceInfo.ESP_firmware_ver, (sizeof(DeviceInfo.ESP_firmware_ver) - 1));
	memcpy(prod_details->STM_firmware_ver, DeviceInfo.STM_firmware_ver, (sizeof(DeviceInfo.STM_firmware_ver) - 1));

	prod_details->PCB_details.PCBA_test_status = DeviceInfo.PCBA_test_status;
	prod_details->PCB_details.PCB_hardware_ver = DeviceInfo.PCB_hardware_ver;
	memcpy(prod_details->PCB_details.PCB_serial_number, DeviceInfo.PCB_serial_number, (sizeof(DeviceInfo.PCB_serial_number) - 1));
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        updatePcbDetails()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		function updates the PCB details received to Device info structure
 * 				and saves device info details into ESP NVS
 *  *
 * @param[in] 	pointer to received data
 * @retval     	Ack / Nack
*/
//--------------------------------------------------------------------------------------------------
responceAckNac_e updatePcbDetails(uint8_t *prx_buffer)
{
	Pcbdetsils_st pcb_details;
	responceAckNac_e ack_nack = ACK;

	memset(&pcb_details, 0, sizeof(Pcbdetsils_st));
	memcpy(&pcb_details, prx_buffer + sizeof(sblcpHeader_st), sizeof(Pcbdetsils_st));


	//update data PCB details in to Device info structure
	DeviceInfo.PCBA_test_status = pcb_details.PCBA_test_status;
	DeviceInfo.PCB_hardware_ver = pcb_details.PCB_hardware_ver;
	memcpy(&DeviceInfo.PCB_serial_number, pcb_details.PCB_serial_number, sizeof(pcb_details.PCB_serial_number));

	//store PCB details into ESP NVS
	WriteDataToFlash(EEOFFSET_SET_PCB_DETAILS, (char *)&pcb_details, sizeof(pcb_details));

#if SBLCP_DEBUG
	ets_printf("PCBA_test_status:-[%d]\n", pcb_details.PCBA_test_status);
	ets_printf("PCB_hardware_ver:-[%d]\n", DeviceInfo.PCB_hardware_ver);
	ets_printf("PCB_serial_number:-[%s]\n", DeviceInfo.PCB_serial_number);
#endif

	return ack_nack;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        get_SblcpCommandStatus()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		function to get the SBLCP command status value.
 * 				if this value is TRUE means this is a SBLCP command
 *  *			if this value is false means this is not a SBLCP command
 * @param[in] 	none
 * @retval     	true / false
*/
//--------------------------------------------------------------------------------------------------
uint8_t get_SblcpCommandStatus()
{
	return replyStatusInfo.sblcp_timeout_command;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        SBLCP_performFactoryReset()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		function performs factory resets.
 * 				resets all UPD keys and initiates Manufacturing reset.
 * @param[in] 	none
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void SBLCP_performFactoryReset()
{
	delete_primaryUnicastudpKeys();
	delete_secondaryUnicastudpKeys();
	delete_primary_BroadcastKey();
	delete_secondary_BroadcastKey();
	ResetDevice(MANUFACTURING_RESET, true);
}
#if SBLCP_DEBUG
 void print_reposnce(void *pMsgForPrint, uint16_t msg_code)
{
	sblcpResponce_st *pRsponce = NULL;
	uint8_t *msg = NULL;

	msg = (uint8_t *)pMsgForPrint;
	pRsponce = (sblcpResponce_st *)pMsgForPrint;

	ets_printf("********Header*********\n");
	ets_printf("start_of_message %x\n", pRsponce->sblcp_header.start_byte);
	ets_printf("sequence_number %x\n", pRsponce->sblcp_header.seq_number);
	ets_printf("message_code %x\n", pRsponce->sblcp_header.mesg_code);
	ets_printf("\n");

	if(msg_code == GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER)
	{
		ets_printf("********Message Body*********\n");
		getSeqNumberMesg_st *pMsg_body = (getSeqNumberMesg_st *)&pRsponce->message_data;

		ets_printf("seq_number %x\n", pMsg_body->seq_number);
		ets_printf("serial_number %s \n", pMsg_body->serial_number);
		ets_printf("\n");
		ets_printf("version %x\n", pMsg_body->version);
		ets_printf("nonce %x\n", pMsg_body->nonce);
		ets_printf("\n");
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(getSeqNumberMesg_st) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == GET_DEVICE_STATUS)
	{

		ets_printf("********Message Body*********\n");
		breakerStatus_st *pMsg_body = (breakerStatus_st *)&pRsponce->message_data;

		ets_printf("handle status %d\n", pMsg_body->breaker_state);
		ets_printf("Primary handle status %d\n", pMsg_body->primary_handle_status);
		ets_printf("Primary handle status %d\n", pMsg_body->path_status);

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



		ets_printf("\n");
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(breakerStatus_st) + i]);
		}
		ets_printf("]\n");
//		ets_printf("meter_update_no %d\n", pMsg_body->meter_update_no);
//		ets_printf("period %d\n", pMsg_body->period);
//		ets_printf("ph0_Q1_act_energy %lld\n", pMsg_body->ph0_Q1_act_energy);
//		ets_printf("ph0_Q1_app_energy %lld\n", pMsg_body->ph0_Q1_app_energy);
//		ets_printf("ph0_Q1_reactive_energy %lld\n", pMsg_body->ph0_Q1_reactive_energy);
//		ets_printf("ph0_Q2_act_energy %lld\n", pMsg_body->ph0_Q2_act_energy);
//		ets_printf("ph0_Q2_app_energy %lld\n", pMsg_body->ph0_Q2_app_energy);
//		ets_printf("ph0_Q2_reactive_energy %lld\n", pMsg_body->ph0_Q2_reactive_energy);
//		ets_printf("ph0_Q3_act_energy %lld\n", pMsg_body->ph0_Q3_act_energy);
//		ets_printf("ph0_Q3_app_energy %lld\n", pMsg_body->ph0_Q3_app_energy);
//		ets_printf("ph0_Q3_reactive_energy %lld\n", pMsg_body->ph0_Q3_reactive_energy);
//		ets_printf("ph0_Q4_act_energy %lld\n", pMsg_body->ph0_Q4_act_energy);
//		ets_printf("ph0_Q4_app_energy %lld\n", pMsg_body->ph0_Q4_app_energy);
//		ets_printf("ph0_Q4_reactive_energy %lld\n", pMsg_body->ph0_Q4_reactive_energy);
//		ets_printf("ph0_Q5_act_energy %lld\n", pMsg_body->ph0_Q5_act_energy);
//		ets_printf("ph0_Q5_app_energy %lld\n", pMsg_body->ph0_Q5_app_energy);
//		ets_printf("ph0_Q5_reactive_energy %lld\n", pMsg_body->ph0_Q5_reactive_energy);
//		ets_printf("ph1_Q1_act_energy %lld\n", pMsg_body->ph1_Q1_act_energy);
//		ets_printf("ph1_Q1_app_energy %lld\n", pMsg_body->ph1_Q1_app_energy);
//		ets_printf("ph1_Q1_reactive_energy %lld\n", pMsg_body->ph1_Q1_reactive_energy);
//		ets_printf("ph1_Q2_act_energy %lld\n", pMsg_body->ph1_Q2_act_energy);
//		ets_printf("ph1_Q2_app_energy %lld\n", pMsg_body->ph1_Q2_app_energy);
//		ets_printf("ph1_Q2_reactive_energy %lld\n", pMsg_body->ph1_Q2_reactive_energy);
//		ets_printf("ph1_Q3_act_energy %lld\n", pMsg_body->ph1_Q3_act_energy);
//		ets_printf("ph1_Q3_app_energy %lld\n", pMsg_body->ph1_Q3_app_energy);
//		ets_printf("ph1_Q3_reactive_energy %lld\n", pMsg_body->ph1_Q3_reactive_energy);
//		ets_printf("ph1_Q4_act_energy %lld\n", pMsg_body->ph1_Q4_act_energy);
//		ets_printf("ph1_Q4_app_energy %lld\n", pMsg_body->ph1_Q4_app_energy);
//		ets_printf("ph1_Q4_reactive_energy %lld\n", pMsg_body->ph1_Q4_reactive_energy);
//		ets_printf("ph1_Q5_act_energy %lld\n", pMsg_body->ph1_Q5_act_energy);
//		ets_printf("ph1_Q5_app_energy %lld\n", pMsg_body->ph1_Q5_app_energy);
//		ets_printf("ph1_Q5_reactive_energy %lld\n", pMsg_body->ph1_Q5_reactive_energy);
		ets_printf("\n");
	}
	if(msg_code == GET_METER_TELEMETRY_DATA)
	{
		ets_printf("********Message Body*********\n");
		meterTelemetryData_st *pMsg_body = (meterTelemetryData_st *)&pRsponce->message_data;

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
		ets_printf("\n");
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(meterTelemetryData_st) + i]);
		}
		ets_printf("]\n");
//		ets_printf("meter_update_no %d\n", pMsg_body->meter_update_no);
//		ets_printf("period %d\n", pMsg_body->period);
//		ets_printf("ph0_Q1_act_energy %lld\n", pMsg_body->ph0_Q1_act_energy);
//		ets_printf("ph0_Q1_app_energy %lld\n", pMsg_body->ph0_Q1_app_energy);
//		ets_printf("ph0_Q1_reactive_energy %lld\n", pMsg_body->ph0_Q1_reactive_energy);
//		ets_printf("ph0_Q2_act_energy %lld\n", pMsg_body->ph0_Q2_act_energy);
//		ets_printf("ph0_Q2_app_energy %lld\n", pMsg_body->ph0_Q2_app_energy);
//		ets_printf("ph0_Q2_reactive_energy %lld\n", pMsg_body->ph0_Q2_reactive_energy);
//		ets_printf("ph0_Q3_act_energy %lld\n", pMsg_body->ph0_Q3_act_energy);
//		ets_printf("ph0_Q3_app_energy %lld\n", pMsg_body->ph0_Q3_app_energy);
//		ets_printf("ph0_Q3_reactive_energy %lld\n", pMsg_body->ph0_Q3_reactive_energy);
//		ets_printf("ph0_Q4_act_energy %lld\n", pMsg_body->ph0_Q4_act_energy);
//		ets_printf("ph0_Q4_app_energy %lld\n", pMsg_body->ph0_Q4_app_energy);
//		ets_printf("ph0_Q4_reactive_energy %lld\n", pMsg_body->ph0_Q4_reactive_energy);
//		ets_printf("ph0_Q5_act_energy %lld\n", pMsg_body->ph0_Q5_act_energy);
//		ets_printf("ph0_Q5_app_energy %lld\n", pMsg_body->ph0_Q5_app_energy);
//		ets_printf("ph0_Q5_reactive_energy %lld\n", pMsg_body->ph0_Q5_reactive_energy);
//		ets_printf("ph1_Q1_act_energy %lld\n", pMsg_body->ph1_Q1_act_energy);
//		ets_printf("ph1_Q1_app_energy %lld\n", pMsg_body->ph1_Q1_app_energy);
//		ets_printf("ph1_Q1_reactive_energy %lld\n", pMsg_body->ph1_Q1_reactive_energy);
//		ets_printf("ph1_Q2_act_energy %lld\n", pMsg_body->ph1_Q2_act_energy);
//		ets_printf("ph1_Q2_app_energy %lld\n", pMsg_body->ph1_Q2_app_energy);
//		ets_printf("ph1_Q2_reactive_energy %lld\n", pMsg_body->ph1_Q2_reactive_energy);
//		ets_printf("ph1_Q3_act_energy %lld\n", pMsg_body->ph1_Q3_act_energy);
//		ets_printf("ph1_Q3_app_energy %lld\n", pMsg_body->ph1_Q3_app_energy);
//		ets_printf("ph1_Q3_reactive_energy %lld\n", pMsg_body->ph1_Q3_reactive_energy);
//		ets_printf("ph1_Q4_act_energy %lld\n", pMsg_body->ph1_Q4_act_energy);
//		ets_printf("ph1_Q4_app_energy %lld\n", pMsg_body->ph1_Q4_app_energy);
//		ets_printf("ph1_Q4_reactive_energy %lld\n", pMsg_body->ph1_Q4_reactive_energy);
//		ets_printf("ph1_Q5_act_energy %lld\n", pMsg_body->ph1_Q5_act_energy);
//		ets_printf("ph1_Q5_app_energy %lld\n", pMsg_body->ph1_Q5_app_energy);
//		ets_printf("ph1_Q5_reactive_energy %lld\n", pMsg_body->ph1_Q5_reactive_energy);
		ets_printf("\n");
	}
	if(msg_code == GET_REMOTE_HANDLE_POSISTION)
	{
		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("Handle status %d\n", pMsg_body[10]);
		ets_printf("\n");
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER)
	{
		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("ACK_NACK %d\n", pMsg_body[10]);
		ets_printf("\n");
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == SET_LED_TO_USER_DEFINED_COLOUR)
	{
		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("ACK_NACK %d\n", pMsg_body[10]);
		ets_printf("\n");
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == SET_REMOTE_HANDLE_POSISTION)
	{

		ets_printf("********Message Body*********\n");
		setHandlePosition_st *pMsg_body = (setHandlePosition_st *)&pRsponce->message_data;
		ets_printf("ACK_NACK %d\n", pMsg_body->responce);
		ets_printf("handle position %x\n", pMsg_body->handle_posistion);
		ets_printf("\n");
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(setHandlePosition_st) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == SET_BREAKER_EOL_CALIB_PARAM)
	{

		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("ACK_NACK %d\n", pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(BreakerEOLCalibration_st) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	if(msg_code == GET_BREAKER_EOL_CALIB_PARAM)
	{
		BreakerEOLCalibration_st *pMsg_body = (BreakerEOLCalibration_st *)&pRsponce->message_data;
		ets_printf("********Message Body*********\n");

		ets_printf("Phase_A voltage Gain:-[%x]\n", pMsg_body->avGain);
		ets_printf("Phase_A Current Gain:-[%x]\n", pMsg_body->aiGain);
		ets_printf("Phase_A RMS current OS:-[%x]\n", pMsg_body->aiRmsOs);
		ets_printf("Phase_A Power Gain:-[%x]\n",pMsg_body->apGain);
		ets_printf("Phase_A cal 0:-[%x]\n", pMsg_body->aphCal_0);

		ets_printf("Phase_B voltage Gain:-[%x]\n", pMsg_body->bvGain);
		ets_printf("Phase_B Current Gain:-[%x]\n", pMsg_body->biGain);
		ets_printf("Phase_B RMS current OS:-[%x]\n", pMsg_body->biRmsOs);
		ets_printf("Phase_B Power Gain:-[%x]\n",pMsg_body->bpGain);
		ets_printf("Phase_B cal 0:-[%x]\n", pMsg_body->bphCal_0);

		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(BreakerEOLCalibration_st) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	if(msg_code == SET_MANUFACTURING_MODE_ON)
	{

		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("ACK_NACK %d\n", pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == SET_MANUFACTURING_MODE_OFF)
	{

		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("ACK_NACK %d\n", pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == RESET_ENERGY)
	{

		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("ACK_NACK %d\n", pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == GET_PRIMARY_HANDLE_STATUS)
	{

		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("Primary Handle status %d\n", pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == SET_MANUFACTURING_RESET)
	{

		ets_printf("********Message Body*********\n");
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("ACK_NACK %d\n", pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
	}
	if(msg_code == SET_PRODUCTION_DETAILS)
	{
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("********Message Body*********\n");

		ets_printf("ACK_NACK %d\n",pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	if(msg_code == GET_PRODUCTION_DETAILS)
	{
		DeviceProductionDetails_st *pMsg_body = (DeviceProductionDetails_st *)&pRsponce->message_data;
		char cat_numner[CATALOG_LENGTH + 1];
		char style_number[STYLE_NUMBER_LENGTH + 1];
		char serial_numbe[SERIAL_NUMBER_LENGTH + 1];
		char ESP_firmware_ver[FIRMWARE_VER_LENGTH + 1];
		char STM_Firmware_ver[FIRMWARE_VER_LENGTH + 1];
		uint8_t pcb_hardware_ver = 0;
		char pcb_serial_no[PCB_SERIAL_NUMBER_LENGTH + 1];

		memset(cat_numner, 0, sizeof(cat_numner));
		memset(style_number, 0, sizeof(style_number));
		memset(serial_numbe, 0, sizeof(serial_numbe));
		memset(ESP_firmware_ver, 0, sizeof(ESP_firmware_ver));
		memset(STM_Firmware_ver, 0, sizeof(STM_Firmware_ver));
		memset(pcb_serial_no, 0, sizeof(pcb_serial_no));

		memcpy(cat_numner, pMsg_body->device_details.catalog_number, sizeof(pMsg_body->device_details.catalog_number));
		memcpy(style_number, pMsg_body->device_details.style_number, sizeof(pMsg_body->device_details.style_number));
		memcpy(serial_numbe, pMsg_body->device_details.breaker_serial_no, sizeof(pMsg_body->device_details.breaker_serial_no));
		memcpy(ESP_firmware_ver, pMsg_body->ESP_firmware_ver, sizeof(pMsg_body->ESP_firmware_ver));
		memcpy(STM_Firmware_ver, pMsg_body->STM_firmware_ver, sizeof(pMsg_body->STM_firmware_ver));
		memcpy(pcb_serial_no, pMsg_body->PCB_details.PCB_serial_number, sizeof(pMsg_body->PCB_details.PCB_serial_number));

		ets_printf("********Message Body*********\n");

		ets_printf("Current Rating:-[%d]\n", pMsg_body->device_details.current_rating);
		ets_printf("breaker_test_status:-[%d]\n", pMsg_body->device_details.breaker_test_status);
		ets_printf("catalog_number:-[%s]\n", cat_numner);
		ets_printf("style_number:-[%s]\n", style_number);
		ets_printf("breaker_serial_no:-[%s]\n", serial_numbe);
		ets_printf("ESP_firmware_ver:-[%s]\n", ESP_firmware_ver);
		ets_printf("STM_firmware_ver:-[%s]\n", STM_Firmware_ver);

		ets_printf("PCBA_test_status:-[%d]\n", pMsg_body->PCB_details.PCBA_test_status);
		ets_printf("PCB_hardware_ver:-[%d]\n", pMsg_body->PCB_details.PCB_hardware_ver);
		ets_printf("PCB_serial_number:-[%s]\n", pcb_serial_no);

		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(DeviceProductionDetails_st) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	if(msg_code == SET_PCB_DETAILS)
	{
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("********Message Body*********\n");

		ets_printf("ACK_NACK %d\n",pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	if(msg_code == SET_DEVICE_LOCK)
	{
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("********Message Body*********\n");

		ets_printf("ACK_NACK %d\n",pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	if(msg_code == IDENTIFY_ME)
	{
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("********Message Body*********\n");

		ets_printf("ACK_NACK %d\n",pMsg_body[10]);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	if(msg_code == GET_MAC_ADDRESS)
	{
		uint8_t *pMsg_body = (uint8_t *)pMsgForPrint;
		ets_printf("********Message Body*********\n");

		ets_printf("MacAddress %s\n",DeviceInfo.MacAddress);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + sizeof(uint8_t) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	if(msg_code == GET_TRIP_LOG)
	{
		ErrorLog_st *pMsg_body = (ErrorLog_st *)&pRsponce->message_data;
		printf("pMsg_body sizee %d \n", sizeof(ErrorLog_st));
		ets_printf("********Message Body*********\n");

		for(uint16_t i = 0; i < TRIP_LOG_SIZE; i++)
		{
			printf("[%x],[%d],[%d] \n", pMsg_body->logs[i].ErrorLog, pMsg_body->logs[i].EpochTime, pMsg_body->logs[i].CloudUpdated);
		}
		printf("Index %d \n", pMsg_body->index);
		ets_printf("Hash [");
		for(uint8_t i = 0; i < 32; i++)
		{
			ets_printf("%x ", msg[10 + (sizeof(ErrorLog_st)) + i]);
		}
		ets_printf("]\n");
		ets_printf("\n");
	}
	ets_printf("*******************End Response*******************\n");
}
#endif
