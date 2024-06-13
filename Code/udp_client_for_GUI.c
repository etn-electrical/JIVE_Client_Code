/* ****************************************************************************************************
 *
 * 		Connected Solutions @ Eaton (Moon Township, PA)
 *
 * 		Author	: Jonathan Tan (jonathantan@eaton.com)
 * 		Date	: 6/14/2023
 *
 * ****************************************************************************************************
 *
 * 		udp_client_for_GUI.c
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
 * 		Note	: How the client code is structured:
 * 				  - This is the main c file
 * 				  - The header file for this file is udp_client_for_GUI.h
 * 				  - This file uses structs defined in SBLCP.h
 * 				  - This file uses crypto functions from Babelfish (psHmac())
 * 				  - This file is called in app_main.cpp
 *
 * 		Docs	: https://confluence-prod.tcc.etn.com/pages/viewpage.action?pageId=299547479#DesktopSoftwareforCommunicatingwithSB2usingSBLCP.-Backendstuff(Ignoreifyouarejustusingthesoftware):
 *
 *****************************************************************************************************/

// Sys/esp/freertos includes
#include <string.h>
#include <stdint.h>
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
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_task_wdt.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "tcpip_adapter.h"

// Babelfish includes
#include "cryptoApi.h"

// Connect to local h file
#include "udp_client_for_GUI.h"
#include "SBLCP.h"

#define CONFIG_EXAMPLE_IPV4

// Set temporary host (breaker) ip address.
#if defined(CONFIG_EXAMPLE_IPV4)
	char* HOST_IP_ADDR = "255.255.255.255";
#elif defined(CONFIG_EXAMPLE_IPV6)
	char* HOST_IP_ADDR = CONFIG_EXAMPLE_IPV6_ADDR;
#else
	char* HOST_IP_ADDR = "";
#endif

#define PORT 32866

uint8_t *psendMsg;
char wifiSSID[100];
char wifiPassword[100];
int errno;
#ifdef HASHING_IMPLEMENTED
char hhhhash[100];
#endif
int broadcasting = 1;	// 1 = broadcasting; 0 = not broadcasting

uint8_t hash[40] = {0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,
		0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde};

uint8_t hashToSend[32];

uint32_t g_getSequenceNumber = 0;
uint32_t nextSequenceNumber;

uint8_t g_manufacturingResetCount = 0;

uint16_t message_code;
uint32_t start_msg;

breakerStatus_st *g_metroData;
getSeqNumberMesg_st *g_getSeqNum;
setHandlePosition_st *g_SetHandlePosistion;
BreakerEOLCalibration_st g_metroCalib;
BreakerMacAddress_st g_macAddress;
uint8_t *g_response;

uint32_t response_counter;	// This will be initialized to 0, everytime we receive a response, we will increment
							// this variable and prints it out so that Java program can capture it and know that it
							// received a new message.

static const char *TAG = "udp_client_for_GUI";

// They will live here for now
char rx_buffer[1024];
char addr_str[128];
int addr_family = 0;
int ip_protocol = 0;
struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
int sock = 0;
struct sockaddr_in dest_addr;
breakerIdentifier_st arrOfBreakerIdentifierStructs[10];
int numberOfBreakersTalkedTo = 0;

int timeOutCounter = 0;

/**
 * The function that would send and receive messages from the SB, as well as do
 * all the printing stuff.
 *
 */
//#define DEBUGGING_udp_client_txrx_task_FUNCTION		// Comment me out when not debugging
void udp_client_txrx_task(void* nothing) {
	// Main while loop
	while (1) {
		// Get wifi credentials
//		getWifiCredentialsFromGUI();	// Blocker function
//		connectToWifi();

#if defined(CONFIG_EXAMPLE_IPV4)
        dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_IPV6)
        dest_addr = { 0 };
        inet6_aton(HOST_IP_ADDR, &dest_addr.sin6_addr);
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(PORT);
        dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_DGRAM, &ip_protocol, &addr_family, &dest_addr));
#endif

        // ---------------------------------------------------------------------------
        // -----	Attempt to open a socket and print error/confirmation
        // ---------------------------------------------------------------------------
        sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
			ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
			break;
		}
        ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);


        // ---------------------------------------------------------------------------
		// -----	Get WiFi Credentials
		// ---------------------------------------------------------------------------
        getWifiCredentialsFromGUIandConnectToWifi();	// Blocking function


        // ---------------------------------------------------------------------------
		// -----	Ping all breakers
		// ---------------------------------------------------------------------------
        int numberOfBreakersPinged = pingAllBreakersOnTheNetwork();
        if (numberOfBreakersPinged >= 1) {
			selectWhichIpAddressToTalkTo(0);
			printf("[INFO] udp_client_txrx_task(): Pinged breaker(s):\n");
			printf("BREAKERS_IP_LIST_JSON_START{");
			for (int i = 0; i < 10; ++i) {
				printf("%d: \"%s\"", i, arrOfBreakerIdentifierStructs[i].ipAddress);
				if (i != 10-1) printf(",");
			}
			printf("}BREAKERS_IP_LIST_JSON_STOP\n");
			sendWifiErrorJSON(0, 0, 0, 0);
//			printf("[DEBUG] udp_client_txrx_task(): Sending Handshake...\n");
//			int status = sendHandshakeMessage();
			// status < 0 mean something is wrong
			// Note: I send "handshake" from GUI by calling the get next udp seq command
        } else {
        	// We did not see any breaker
        	printf("[INFO] udp_client_txrx_task(): Didn't pinged any breaker on the network, continuing to broadcast messages.\n");
        	selectWhichIpAddressToTalkTo(-1);
        }


        // ---------------------------------------------------------------------------
		// -----	Start Listening to Key Presses
		// ---------------------------------------------------------------------------
        char keyPressed = '\0';

        // Initialize metro data struct
        g_metroData = (breakerStatus_st *)malloc(sizeof(breakerStatus_st));
        // Initialize response_counter
        response_counter = 0;

        // Key press listening while loop
        while (1) {
        	// Reset the variable so it will ask for keyboard input again
			keyPressed = '\0';

			scanf("%c", &keyPressed);
			if (keyPressed != '\0') {
				// If got a key press (aka non null, aka != '\0'), output
#ifdef DEBUGGING_udp_client_txrx_task_FUNCTION
				printf("[DEBUG] udp_client_for_GUI.c:udp_client_txrx_task(): Received keyPressed of: %c\n", keyPressed);
#endif
			}

			// Honestly no idea what this is doing
			dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);

			if(keyPressed == '1') {
				send_SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER_message();
			} else if (keyPressed == 'a') {
				send_SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER_message();
			} else if (keyPressed == '2') {
				send_SEND_GET_REMOTE_HANDLE_POSISTION_message();
			} else if (keyPressed == 'e') {
				send_SEND_GET_PRIMARY_HANDLE_STATUS_message();
			} else if (keyPressed == '5') {
				send_SEND_SET_REMOTE_HANDLE_POSISTION_OPEN_message();
			} else if (keyPressed == 'b') {
				send_SEND_SET_REMOTE_HANDLE_POSISTION_CLOSE_message();
			} else if (keyPressed == 'c') {
				send_SEND_SET_REMOTE_HANDLE_POSISTION_TOGGLE_message();
			} else if (keyPressed == '3') {
				send_SEND_SET_MANUFACTURING_MODE_ON_message();
			} else if (keyPressed == '9') {
				send_SEND_SET_MANUFACTURING_MODE_OFF_message();
			} else if (keyPressed == '4') {
				send_SEND_GET_DEVICE_STATUS_message();
			} else if (keyPressed == 'f') {
				send_SEND_GET_METER_TELEMETRY_DATA_message();
			} else if (keyPressed == '6') {
				send_SEND_SET_LED_TO_USER_DEFINED_COLOUR_message();
			} else if (keyPressed == '8') {
				send_SEND_GET_BREAKER_EOL_CALIB_PARAM_message();
			} else if (keyPressed == '7') {
				send_SEND_SET_BREAKER_EOL_CALIB_PARAM_message();
			} else if (keyPressed == 'd') {
				send_SEND_RESET_ENERGY_message();
			} else if (keyPressed == 'z') {
				send_customMessage();
			} else if (keyPressed == 'k') {
				send_MIKE_ENDURANCE_TEST_START();
			} else if (keyPressed == 'l') {
				send_MIKE_ENDURANCE_TEST_STOP();
			} else if (keyPressed == ')') {
				esp_restart();
			} else if (keyPressed == '(') {
				send_SEND_GET_TRIP_CURVE();
			} else if (keyPressed == '-') {
				send_SEND_GET_MAC_ADDRESS();
			}else if (keyPressed == '#') {
				esp_restart();
			} else if (keyPressed == '@') {
				send_SEND_MANUFACTRUNG_RESET();
			}else if (keyPressed == '-'){
				send_SEND_DEVICE_LOCK();
			}
			else {
				if (keyPressed == 'Q') selectWhichIpAddressToTalkTo(0);
				else if (keyPressed == 'W') selectWhichIpAddressToTalkTo(1);
				else if (keyPressed == 'E') selectWhichIpAddressToTalkTo(2);
				else if (keyPressed == 'R') selectWhichIpAddressToTalkTo(3);
				else if (keyPressed == 'T') selectWhichIpAddressToTalkTo(4);
				else if (keyPressed == 'Y') selectWhichIpAddressToTalkTo(5);
				else if (keyPressed == 'U') selectWhichIpAddressToTalkTo(6);
				else if (keyPressed == 'I') selectWhichIpAddressToTalkTo(7);
				else if (keyPressed == 'O') selectWhichIpAddressToTalkTo(8);
				else if (keyPressed == 'P') selectWhichIpAddressToTalkTo(9);

				else if (keyPressed == 'A') send_SEND_IDENTIFY_ME(0);
				else if (keyPressed == 'S') send_SEND_IDENTIFY_ME(1);
				else if (keyPressed == 'D') send_SEND_IDENTIFY_ME(2);
				else if (keyPressed == 'F') send_SEND_IDENTIFY_ME(3);
				else if (keyPressed == 'G') send_SEND_IDENTIFY_ME(4);
				else if (keyPressed == 'H') send_SEND_IDENTIFY_ME(5);
				else if (keyPressed == 'J') send_SEND_IDENTIFY_ME(6);
				else if (keyPressed == 'K') send_SEND_IDENTIFY_ME(7);
				else if (keyPressed == 'L') send_SEND_IDENTIFY_ME(8);
				else if (keyPressed == 'Z') send_SEND_IDENTIFY_ME(9);

				dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);

				// If no key was pressed, keep waiting
				continue;
			}
			
			listenToBreakerResponseAndPrintResponse();
        }

        if (sock != -1) {
			ESP_LOGE(TAG, "Shutting down socket and restarting...");
			shutdown(sock, 0);
			close(sock);
		}
	}

	vTaskDelete(NULL);

	return;
}

/**
 * Main function
 */
void app_main_client(void) {
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	xTaskCreate(udp_client_txrx_task, "udp_client_txrx_task", 4096, NULL, 5, NULL);
}


// ##############################################################################################
// ##############################################################################################
// #################### Below are all helper functions #########################################
// ##############################################################################################
// ##############################################################################################

//#define DEBUGGING_getWifiCredentialsFromGUIandConnectToWifi_FUNCTION
void getWifiCredentialsFromGUIandConnectToWifi() {
	while (1) {
		int weGotTheWifiSSIDandPassword = getWifiCredentialsFromGUI();

		if (!weGotTheWifiSSIDandPassword) {
			vTaskDelay(pdMS_TO_TICKS(2000));
			continue;
		}

		ESP_LOGI(TAG, "\n\n[INFO] udp_client_for_GUI.c:udp_client_txrx_task(): Connecting to WiFi with SSID: %s; Pass: %s.\n", wifiSSID, wifiPassword);
		int weAreConnectedToWifi = connectToWifi(wifiSSID, wifiPassword, 15);

		if (!weAreConnectedToWifi) {
#ifdef DEBUGGING_getWifiCredentialsFromGUIandConnectToWifi_FUNCTION
			printf("[DEBUG] connectToWifi(): Wifi connection failed, rebooting...\n");
#endif

			// Send error
			sendWifiErrorJSON(1, 1, 0, 1);

			// Restart
			esp_restart();
		}

		break;
	}
}

/**
 * This functions writes to variables wifiSSID and wifiPassword.
 * Return:
 *  1 - success
 *  0 - fail
 */
#define DEBUGGING_getWifiCredentialsFromGUI_FUNCTION	// For some weird reason (that I dont know of) commenting out this will create duplicated chars, so DO NOT COMMENT THIS LINE!
//#define DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
#ifdef HASHING_IMPLEMENTED	// See the h file line ~452
int getWifiCredentialsFromGUI() {
	int toReturn = 0;

#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Listening to wifi credentials...\n");
#endif

	char message_string[200] = "";

	char previous;
	int currentSameAsPreviousCounter = 0;
	const int MAX_REPEAT_CHAR_ALLOWED = 10;

	// Listen to comma delimited string of wifi credentials, string format: <junk>,<ssid>,<password>
	char current;
	while (current!= '\n') {
		scanf("%c", &current);

		// --------------- Error Checks ---------------
		if (!(current >= 32 && current <= 126)) {
			// if we got weird characters skip/ignore them
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Weird char: %c\n", current);
#endif
			continue;
		}
		if (currentSameAsPreviousCounter == 0) {
			// Copy current into previous on first time
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): currentSameAsPreviousCounter == 0\n");
#endif
			previous = current;
			++currentSameAsPreviousCounter;
		} else {
			// On not first time, count up if repeat
			if (previous == current) {
				++currentSameAsPreviousCounter;
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
				printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Not first time, counting up: %d.\n", currentSameAsPreviousCounter);
#endif
			} else {
				// No more previous == current, reset test
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
				printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): We didn't see max same char, resetting test...\n");
#endif
				previous = current;
				currentSameAsPreviousCounter = 0;
			}
		}
		if (currentSameAsPreviousCounter > MAX_REPEAT_CHAR_ALLOWED) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Max same char saw hit! Sending Error...\n");
#endif
			sendWifiErrorJSON(1, 1, 0, 0);
			break;
		}

		strncat(message_string, &current, 1);

#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
		printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current char: %c\n", current);
#endif
	}

#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
	printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): String I got: %s\n", message_string);
#endif

	char* stringToken = strtok(message_string, ",");
	int stringTokenIndex = 0;
	while (stringToken != NULL) {
//			printf("[DEBUG NEW C CODE] Current String Token: %s\n", stringToken);
		if (stringTokenIndex == 0) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			// Do nothing hehe
		} else if (stringTokenIndex == 1) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			strcpy(wifiSSID, stringToken);
		} else if (stringTokenIndex == 2) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			strcpy(wifiPassword, stringToken);
		} else if (stringTokenIndex == 3) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			strcpy(hhhhash, stringToken);
		} else if (stringTokenIndex == 4) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			// This token is junk + nl char which we dont care
			// Done
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): The wifi ssid is: %s\n", wifiSSID);
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): The wifi pass is: %s\n", wifiPassword);
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): UDP KEY is: %s\n", hhhhash);
#endif
			toReturn = 1;
			break;
		}

		stringToken = strtok(NULL, ",");
		++stringTokenIndex;
	}

	if (stringTokenIndex != 4) {
		// Something went horribly wrong

		sendWifiErrorJSON(1, 1, 0, 0);

		toReturn = 0;
	}

	return toReturn;
}
#else
int getWifiCredentialsFromGUI() {
	int toReturn = 0;

#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Listening to wifi credentials...\n");
#endif

	char message_string[200] = "";

	char previous;
	int currentSameAsPreviousCounter = 0;
	const int MAX_REPEAT_CHAR_ALLOWED = 10;

	// Listen to comma delimited string of wifi credentials, string format: <junk>,<ssid>,<password>
	char current;
	while (current!= '\n') {
		scanf("%c", &current);

		// --------------- Error Checks ---------------
		if (!(current >= 32 && current <= 126)) {
			// if we got weird characters skip/ignore them
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Weird char: %c\n", current);
#endif
			continue;
		}
		if (currentSameAsPreviousCounter == 0) {
			// Copy current into previous on first time
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): currentSameAsPreviousCounter == 0\n");
#endif
			previous = current;
			++currentSameAsPreviousCounter;
		} else {
			// On not first time, count up if repeat
			if (previous == current) {
				++currentSameAsPreviousCounter;
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
				printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Not first time, counting up: %d.\n", currentSameAsPreviousCounter);
#endif
			} else {
				// No more previous == current, reset test
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
				printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): We didn't see max same char, resetting test...\n");
#endif	
				previous = current;
				currentSameAsPreviousCounter = 0;
			}
		}
		if (currentSameAsPreviousCounter > MAX_REPEAT_CHAR_ALLOWED) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Max same char saw hit! Sending Error...\n");
#endif
			sendWifiErrorJSON(1, 1, 0, 0);
			break;
		}

		strncat(message_string, &current, 1);

#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
		printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current char: %c\n", current);
#endif
	}

#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION_2
	printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): String I got: %s\n", message_string);
#endif

	char* stringToken = strtok(message_string, ",");
	int stringTokenIndex = 0;
	while (stringToken != NULL) {
//			printf("[DEBUG NEW C CODE] Current String Token: %s\n", stringToken);
		if (stringTokenIndex == 0) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			// Do nothing hehe
		} else if (stringTokenIndex == 1) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			strcpy(wifiSSID, stringToken);
		} else if (stringTokenIndex == 2) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			strcpy(wifiPassword, stringToken);
		} else if (stringTokenIndex == 3) {
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): Current token: %s\n", stringToken);
#endif
			// This token is junk + nl char which we dont care
			// Done
#ifdef DEBUGGING_getWifiCredentialsFromGUI_FUNCTION
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): The wifi ssid is: %s\n", wifiSSID);
			printf("[DEBUG] udp_client_for_GUI.c:getWifiCredentialsFromGUI(): The wifi pass is: %s\n", wifiPassword);
#endif
			toReturn = 1;
			break;
		}

		stringToken = strtok(NULL, ",");
		++stringTokenIndex;
	}

	if (stringTokenIndex != 3) {
		// Something went horribly wrong

		sendWifiErrorJSON(1, 1, 0, 0);

		toReturn = 0;
	}

	return toReturn;
}
#endif

void sendWifiErrorJSON(int SSID_Error, int Pass_Error, int Send_Error, int Other_Error) {
	// Send WiFi Error JSON
	printf("WIFI_ERROR_JSON_START{");
	printf("\"WiFi_SSID_Error\": %d,", SSID_Error);
	printf(" \"WiFi_Pass_Error\": %d,", Pass_Error);
	printf(" \"WiFi_Send_Error\": %d,", Send_Error);
	printf(" \"WiFi_Other_Error\": %d", Other_Error);
	printf("}WIFI_ERROR_JSON_END\n");
}

/**
 * Return:
 *  1 - success
 *  0 - fail
 */
int connectToWifi(char* givenSSID, char* givenPassword, int MAX_RETRY_ATTEMPS) {
    // Initialize the Wi-Fi module
	char *desc = "\0";
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();

    // Initialize IP address with 0.0.0.0 aka 0x0
    esp_netif_ip_info_t ip_info;
	ip_info.ip.addr = 0;  // Example IP address: 0.0.0.0
	esp_netif_config.ip_info = &ip_info;


	asprintf(&desc, "%s: %s", TAG, esp_netif_config.if_desc);
    esp_netif_config.if_desc = desc;
    esp_netif_config.route_prio = 128;
    esp_netif_t *netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);

    esp_wifi_set_default_wifi_sta_handlers();

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));


    for (int attempt = 0; attempt < MAX_RETRY_ATTEMPS; ++attempt) {
		wifi_config_t wifi_config = {
			.sta = {
				.ssid = "",
				.password = "",
			},
		};
		ESP_LOGI(TAG, "Connecting to %s...", givenSSID);

		strncpy((char*)wifi_config.sta.ssid, givenSSID, sizeof(wifi_config.sta.ssid) - 1);
		strncpy((char*)wifi_config.sta.password, givenPassword, sizeof(wifi_config.sta.password) - 1);

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
		ESP_ERROR_CHECK(esp_wifi_start());
		esp_wifi_connect();


		const int MAX_WAIT_TIME_MS = 4500;
		const int INTERVAL_WAIT_TIME_MS = 100;
		int waited_ms = 0;
		while (waited_ms <= MAX_WAIT_TIME_MS) {
			ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));

			if (ip_info.ip.addr != 0) {
				printf("[DEBUG] connectToWifi(): We got wifi! IP is: %d.%d.%d.%d\n",
					IP2STR(&ip_info.ip));

				printf("[DEBUG] connectToWifi(): Took us %d ms.\n", waited_ms);
				printf("####################################################################################################################################\n\n");
				return 1;	// Return success
			}

			vTaskDelay(pdMS_TO_TICKS(INTERVAL_WAIT_TIME_MS));
			waited_ms += INTERVAL_WAIT_TIME_MS;
		}

		// Yelp I guess we failed to connect to wifi... :(
		if (waited_ms > MAX_WAIT_TIME_MS) {
			printf("[INFO] connectToWifi(): Wifi connection timeout. Retrying...\n");
		}
    }

	return 0;
}

int pingAllBreakersOnTheNetwork() {
	broadcasting = 1;

	send_SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER_message();

	for (int i = 0; i < 10; ++i) {
		int listenResponseCode = listenToBreakerResponseAndPrintResponse();

		if (listenResponseCode == -2) {
//			printf("[DEBUG] pingAllBreakersOnTheNetwork(): Timed out, probably means we pinged all breakers on the network.\n");
			break;
		}
	}

	broadcasting = 0;
	return numberOfBreakersTalkedTo;
}

/**
 * Given the index for arrOfBreakerIdentifierStructs, assign the IP address of that
 * index and assign that to HOST_IP_ADDR so all messages moving forward will talk to
 * that IP address.
 */
#define DEBUGGING_selectWhichIpAddressToTalkTo_FUNCTION
void selectWhichIpAddressToTalkTo(int index) {
	if (index >= 0) {
		HOST_IP_ADDR = arrOfBreakerIdentifierStructs[index].ipAddress;

#ifdef DEBUGGING_selectWhichIpAddressToTalkTo_FUNCTION
		printf("[DEBUG] selectWhichIpAddressToTalkTo(): Updating Target IP Address to: %s\n", HOST_IP_ADDR);
#endif
	} else {
		HOST_IP_ADDR = "255.255.255.255";
	}
}

/**
 * "Handshake message" referes to the SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER
 * command. Because the response include the serial number of the breaker and the
 * host (breaker) ip address, we can grab those data.
 *
 * This function send the SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER command and saves
 * the serial number and the host ip address into the respective variables.
 *
 */
int sendHandshakeMessage() {
	int status = send_SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER_message();
	int fail = listenToBreakerResponseAndPrintResponse();

	if (fail != -1) {
		sendWifiErrorJSON(0, 0, 0, 0);	// No error
	}

	return status;
}

/**
 *  This function will take in a message code and message body (if there is one), pack it up with the header,
 *  hash it, and sends it.
 *
 *  This is the one and only function that does message sending.
 */
//#define DEBUGGING_send_SBLCP_message_FUNCTION	// Comment me out when not used
int send_SBLCP_message(uint16_t givenMessageCode, void* givenMessageBody) {
	// Calculated total message size
	int messageLength_B = 0;
	int messageBodyLenIfThereIsOne = 0;
	if (givenMessageBody != NULL) {
		messageLength_B = HEADER_OFFSET/*which is 10B*/;
		switch (givenMessageCode) {
			case SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER:
				messageLength_B += sizeof(uint32_t);
				break;
			case SEND_SET_LED_TO_USER_DEFINED_COLOUR:
				messageLength_B += sizeof(uint32_t);
				break;
			case SEND_SET_BREAKER_EOL_CALIB_PARAM:
				messageLength_B += sizeof(BreakerEOLCalibration_st);
				break;
			case SEND_SET_REMOTE_HANDLE_POSISTION:
				messageLength_B += sizeof(uint8_t);
				break;
			default:
				break;
		}
		messageLength_B += sizeof(hashToSend)/*which is 32B*/;
		messageBodyLenIfThereIsOne = messageLength_B - HEADER_OFFSET - sizeof(hashToSend);
#ifdef DEBUGGING_send_SBLCP_message_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:send_SBLCP_message(): message body sizeof: %d.\n", messageBodyLenIfThereIsOne);
#endif
	} else {
		messageLength_B = HEADER_OFFSET/*which is 10B*/ + sizeof(hashToSend)/*which is 32B*/;
	}
#ifdef DEBUGGING_send_SBLCP_message_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:send_SBLCP_message(): messageLength_B is: %d Bytes\n", messageLength_B);
#endif

	// Create array to be sent
	uint8_t psendMsg[messageLength_B];
	
	// Log Message Code
	switch (givenMessageCode) {
		case SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER:
			printf("Sending GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER command");
			break;
		case SEND_GET_DEVICE_STATUS:
			printf("Sending GET_DEVICE_STATUS command");
			break;
		case SEND_GET_REMOTE_HANDLE_POSISTION:
			printf("Sending GET_REMOTE_HANDLE_POSISTION command");
			break;
		case SEND_GET_PRIMARY_HANDLE_STATUS:
			printf("Sending GET_PRIMARY_HANDLE_STATUS command");
			break;
		case SEND_GET_METER_TELEMETRT_DATA:
			printf("Sending GET_METER_TELEMETRT_DATA command");
			break;
		case SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER:
			printf("Sending SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER command");
			break;
		case SEND_SET_REMOTE_HANDLE_POSISTION:
			printf("Sending SET_REMOTE_HANDLE_POSISTION command");
			break;
		case SEND_SET_LED_TO_USER_DEFINED_COLOUR:
			printf("Sending SET_LED_TO_USER_DEFINED_COLOUR command");
			break;
		case SEND_SET_BREAKER_EOL_CALIB_PARAM:
			printf("Sending SET_BREAKER_EOL_CALIB_PARAM command");
			break;
		case SEND_GET_BREAKER_EOL_CALIB_PARAM:
			printf("Sending GET_BREAKER_EOL_CALIB_PARAM command");
			break;
		case SEND_SET_MANUFACTURING_MODE_ON:
			printf("Sending SET_MANUFACTURING_MODE_ON command");
			break;
		case SEND_SET_MANUFACTURING_MODE_OFF:
			printf("Sending SET_MANUFACTURING_MODE_OFF command");

			break;
		case SEND_RESET_ENERGY:
			printf("Sending RESET_ENERGY command");
			if((g_manufacturingResetCount == 0) || (g_manufacturingResetCount == 1))
			{
				printf("Sending RESET_ENERGY command");
			}
			if(g_manufacturingResetCount == 2)
			{
				printf("Sending SET_MANUFACTURING_RESET command");
			}
			break;
			case SEND_GET_TRIP_CURVE:
			printf("Sending TRIP_CURVE command");
			break;
		case SEND_GET_MAC_ADDRESS:
			printf("Sending MAC_ADDRESS command");
		break;
		case SEND_SET_MANUFACTURING_RESET:
			printf("Sending ET_MANUFACTURING_RESET command");
		break;
		default:
			printf("[WARNING] udp_client_for_GUI.c:send_SBLCP_message(): Sending Unknown command");
			break;
	}
	printf(" to %s.\n", HOST_IP_ADDR);	// Print out the IP too

	// Insert Start of Message (0x4d4e5445) into message
	uint32_t startOfMessage = STAR_OF_MESSAGE;
	memcpy(&psendMsg[0], &startOfMessage, 4);
#ifdef DEBUGGING_send_SBLCP_message_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:send_SBLCP_message(): Start of Message Inserted. (0x%08x)\n", startOfMessage);
#endif
//	printf("\n[INFO] udp_client_for_GUI.c:send_SBLCP_message(): psendMsg => [");
//	for (int i = 0; i < messageLength_B; ++i) {
//		printf("%02x", psendMsg[i]);
//		if (i != messageLength_B - 1) printf(" ");
//	}
//	printf("]\n\n");
	
	// Insert Sequence Number
//	g_getSequenceNumber += 1;
	memcpy(&psendMsg[4], &g_getSequenceNumber, 4);
#ifdef DEBUGGING_send_SBLCP_message_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:send_SBLCP_message(): Sequence Number Inserted. (0x%08x)\n", g_getSequenceNumber);
#endif
	uint32_t littleEndianizedSequenceNumber = EXTRACT_REV_DATA_4_BYTE(psendMsg, 4);	// This flip the number into little endian 0x12345678 => 0x78563412
	memcpy(psendMsg + sizeof(uint32_t), &littleEndianizedSequenceNumber, sizeof(uint32_t));
//	printf("\n[INFO] udp_client_for_GUI.c:send_SBLCP_message(): psendMsg => [");
//	for (int i = 0; i < messageLength_B; ++i) {
//		printf("%02x", psendMsg[i]);
//		if (i != messageLength_B - 1) printf(" ");
//	}
//	printf("]\n\n");

	// Insert Message Code
	memcpy(&psendMsg[8], &givenMessageCode, 2);
#ifdef DEBUGGING_send_SBLCP_message_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:send_SBLCP_message(): Message Code Inserted. (0x%04x)\n", givenMessageCode);
#endif
	uint16_t littleEndianizedMessageCode = EXTRACT_DATA_2_BYTE(psendMsg, 8);
	memcpy(&psendMsg[8], &littleEndianizedMessageCode, 2);
//	printf("\n[INFO] udp_client_for_GUI.c:send_SBLCP_message(): psendMsg => [");
//	for (int i = 0; i < messageLength_B; ++i) {
//		printf("%02x", psendMsg[i]);
//		if (i != messageLength_B - 1) printf(" ");
//	}
//	printf("]\n\n");

	// Insert Message Body (If there is one)
	if (givenMessageBody != NULL) {
		memcpy(&psendMsg[10], givenMessageBody, messageBodyLenIfThereIsOne);
//		printf("\n[INFO] udp_client_for_GUI.c:send_SBLCP_message(): psendMsg => [");
//		for (int i = 0; i < messageLength_B; ++i) {
//			printf("%02x", psendMsg[i]);
//			if (i != messageLength_B - 1) printf(" ");
//		}
//		printf("]\n\n");#ifdef DEBUGGING_send_SBLCP_message_FUNCTION
#ifdef DEBUGGING_send_SBLCP_message_FUNCTION
		printf("[DEBUG] udp_client_for_GUI.c:send_SBLCP_message(): Message Body Inserted.\n");
#endif
	}

	//if(messageBodyLenIfThereIsOne == 0)
	//{
		messageBodyLenIfThereIsOne = messageLength_B - sizeof(hashToSend);
		printf("[DEBUG] udp_client_for_GUI.c:messageBodyLenIfThereIsOne = [%d]\n", (messageBodyLenIfThereIsOne));
		// Calculate Hash
		modifyHashToSendArrayUsingGivenPsendMsg(psendMsg, messageLength_B, messageBodyLenIfThereIsOne);
	//}
//	else
//	{
//		printf("[DEBUG] udp_client_for_GUI.c:messageLength_B - messageBodyLenIfThereIsOne = [%d]\n", (messageLength_B - messageBodyLenIfThereIsOne));
//		// Calculate Hash
//		modifyHashToSendArrayUsingGivenPsendMsg(psendMsg, messageLength_B, messageLength_B - messageBodyLenIfThereIsOne);
//	}
	// Insert Hash
	memcpy(&psendMsg[messageLength_B - sizeof(hashToSend)], hashToSend, sizeof(hashToSend));
//	printf("\n[INFO] udp_client_for_GUI.c:send_SBLCP_message(): psendMsg => [");
//	for (int i = 0; i < messageLength_B; ++i) {
//		printf("%02x", psendMsg[i]);
//		if (i != messageLength_B - 1) printf(" ");
//	}
//	printf("]\n\n");
#ifdef DEBUGGING_send_SBLCP_message_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:send_SBLCP_message(): Hash Inserted.\n");
#endif

	// Logging
	printf("Start of message:-[0x%08x]\n", startOfMessage);
	printf("Sequence Number:-[0x%08x]\n", g_getSequenceNumber);
	printf("Message Code:-[0x%04x]\n", givenMessageCode);
	printf("Hash:-[");
	for (int i = 0; i < 32; ++i) {
		printf("%02x", hashToSend[i]);
		if (i != 31) printf(" ");
	}
	printf("]\n");

	printf("\n[INFO] udp_client_for_GUI.c:send_SBLCP_message(): psendMsg => [");
	for (int i = 0; i < messageLength_B; ++i) {
		printf("%02x", psendMsg[i]);
		if (i != messageLength_B - 1) printf(" ");
	}
	printf("]\n\n");


	// Ok, finally, send the god damn message
	int err = sendto(sock, psendMsg, messageLength_B, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	
	// Handle err
	if (err < 0) {
		ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
		if (errno == 118) {
			sendWifiErrorJSON(0, 0, 1, 0);
			getWifiCredentialsFromGUIandConnectToWifi();
		}
		return err;
	}

	ESP_LOGI(TAG, "Message sent!");
	return 1;
}

/**
 * As the name suggests, this function will modify the hashToSend array given psendMsg (without hash).
 */
#define DEBUGGING_modifyHashToSendArrayUsingGivenPsendMsg_FUNCTION	// Comment out if not debugging
void modifyHashToSendArrayUsingGivenPsendMsg(uint8_t* psendMsg, int messageLength_B, int messageLendth_wo_hash_B) {

	//messageLendth_wo_hash_B = 10;

#ifdef DEBUGGING_modifyHashToSendArrayUsingGivenPsendMsg_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:modifyHashToSendArrayUsingGivenPsendMsg(): psendMsg to be hashed is: 0x[");
	for (int i = 0; i < messageLength_B - 32; ++i) {
		printf("%02x", psendMsg[i]);
		if (i != messageLength_B - 32 - 1) printf(" ");
	}
	printf("]\n");
	printf("[DEBUG] udp_client_for_GUI.c:modifyHashToSendArrayUsingGivenPsendMsg(): messageLength_B: %d\n", messageLength_B);
	printf("[DEBUG] udp_client_for_GUI.c:modifyHashToSendArrayUsingGivenPsendMsg(): messageLendth_wo_hash_B: %d\n", messageLendth_wo_hash_B);
#endif

	memset(hashToSend, 0x00, sizeof(hashToSend));

//	// "Initialize" the array with nonsense data
//	for (int i = 0; i < 32; ++i) {
//		hashToSend[i] = hash[i];
//	}

	unsigned char udp_key[CRYPTO_HASH_LENGTH + 1]/* = "ba32b432c6e911edafa10242ac120002"*/;
	memcpy(udp_key, hhhhash, CRYPTO_HASH_LENGTH);
	udp_key[CRYPTO_HASH_LENGTH] = '\0';
#ifdef DEBUGGING_modifyHashToSendArrayUsingGivenPsendMsg_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:modifyHashToSendArrayUsingGivenPsendMsg(): Hashing message using key: [%s].\n", udp_key);
	printf("[DEBUG] udp_client_for_GUI.c:modifyHashToSendArrayUsingGivenPsendMsg(): The hash before psHmac is: [");
		for (int i = 0; i < 32; ++i) {
			printf("%02x", hashToSend[i]);
			if (i != 31) printf(" ");
		}
		printf("]\n");
#endif
	psSize_t udp_keyLen = 32;

	// Generate hash
	psHmac(HMAC_SHA256, udp_key, udp_keyLen, psendMsg, messageLendth_wo_hash_B, hashToSend);

#ifdef DEBUGGING_modifyHashToSendArrayUsingGivenPsendMsg_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:modifyHashToSendArrayUsingGivenPsendMsg(): The hash after psHmac is: [");
	for (int i = 0; i < 32; ++i) {
		printf("%02x", hashToSend[i]);
		if (i != 31) printf(" ");
	}
	printf("]\n");
#endif

//	// Add to psendMsg
//	for (int i = 0; i < 32; ++i) {
//		hashToSend[i] = hash[i];
//	}

	return;
}

/**
 *
 *
 * @return 1 = no error. -1 = error occured when sending message.
 */
int send_SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER_message() {
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER, NULL);

	return 1;
}

int send_SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER_message() {
	uint32_t hi = 4096; // aka 2^12
	uint32_t lalala = g_getSequenceNumber + hi;
	uint32_t flippedLalala = bigEndianToLittleEndian32bits(lalala);
	g_manufacturingResetCount = 0;

	send_SBLCP_message(SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER, &flippedLalala);

	g_getSequenceNumber = lalala;

 	return 1;
}

int send_SEND_GET_REMOTE_HANDLE_POSISTION_message() {
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_GET_REMOTE_HANDLE_POSISTION, NULL);

	return 1;
}

int send_SEND_GET_PRIMARY_HANDLE_STATUS_message() {
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_GET_PRIMARY_HANDLE_STATUS, NULL);

	return 1;
}

int send_SEND_SET_REMOTE_HANDLE_POSISTION_OPEN_message() {
	// This opens the handle
	uint8_t openCode = 0x01;
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_SET_REMOTE_HANDLE_POSISTION, &openCode);

	return 1;
}

int send_SEND_SET_REMOTE_HANDLE_POSISTION_CLOSE_message() {
	uint8_t closeCode = 0x03;
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_SET_REMOTE_HANDLE_POSISTION, &closeCode);

	return 1;
}

int send_SEND_SET_REMOTE_HANDLE_POSISTION_TOGGLE_message() {
	// This toggles the handle
	uint8_t toggleCode = 0x02;
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_SET_REMOTE_HANDLE_POSISTION, &toggleCode);

	return 1;
}

int send_SEND_SET_MANUFACTURING_MODE_ON_message() {
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_SET_MANUFACTURING_MODE_ON, NULL);

	return 1;
}

int send_SEND_SET_MANUFACTURING_MODE_OFF_message() {

	send_SBLCP_message(SEND_SET_MANUFACTURING_MODE_OFF, NULL);
//	if((g_manufacturingResetCount == 0) || (g_manufacturingResetCount == 1))
//	{
//		send_SBLCP_message(SEND_SET_MANUFACTURING_MODE_OFF, NULL);
//	}
//	if(g_manufacturingResetCount == 2)
//	{
//		send_SBLCP_message(SET_MANUFACTURING_RESET, NULL);
//		g_manufacturingResetCount = 0;
//	}
//	g_manufacturingResetCount++;
	return 1;
}

int send_SEND_GET_DEVICE_STATUS_message() {
	send_SBLCP_message(SEND_GET_DEVICE_STATUS, NULL);

	return 1;
}

int send_SEND_GET_TRIP_CURVE() {
	send_SBLCP_message(SEND_GET_TRIP_CURVE, NULL);

	return 1;
}

int send_SEND_GET_MAC_ADDRESS() {
	send_SBLCP_message(SEND_GET_MAC_ADDRESS, NULL);

	return 1;
}

int send_SEND_MANUFACTRUNG_RESET() {
	send_SBLCP_message(SEND_SET_MANUFACTURING_RESET, NULL);

	return 1;
}
int send_SEND_DEVICE_LOCK(){
	send_SBLCP_message(SEND_SET_DEVICE_LOCK, NULL);

	return 1;
}

//int send_SEND_IDENTIFY_ME(uint16_t index) {
//    char* OLD_HOST_IP_ADDR = HOST_IP_ADDR;
//    HOST_IP_ADDR = arrOfBreakerIdentifierStructs[index].ipAddress;
//    send_SBLCP_message(SEND_IDENTIFY_ME, NULL);
//    HOST_IP_ADDR = OLD_HOST_IP_ADDR;
//    return 1;
//}

void send_SEND_IDENTIFY_ME(int index) {

	
    char* OLD_HOST_IP_ADDR = HOST_IP_ADDR;
    HOST_IP_ADDR = arrOfBreakerIdentifierStructs[index].ipAddress;
	dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
	send_SBLCP_message(SEND_IDENTIFY_ME, NULL);
    HOST_IP_ADDR = OLD_HOST_IP_ADDR;
}

int send_SEND_GET_METER_TELEMETRY_DATA_message() {
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_GET_METER_TELEMETRY_DATA, NULL);

	return 1;
}



int send_SEND_SET_LED_TO_USER_DEFINED_COLOUR_message() {
	uint8_t ledColor = 0x00;
	uint8_t blinkingTimeInterval = 0x05;
	uint8_t blinkingTimeExpire = 0xff;
	uint8_t blinkingLedFlag = 0x01;

	uint8_t ledSettings[4] = {ledColor, blinkingTimeInterval, blinkingTimeExpire, blinkingLedFlag};

	memcpy(ledSettings, &ledColor, 1);
	memcpy(ledSettings + 1, &blinkingTimeInterval, 1);
	memcpy(ledSettings + 2, &blinkingTimeExpire, 1);
	memcpy(ledSettings + 3, &blinkingLedFlag, 1);

	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_SET_LED_TO_USER_DEFINED_COLOUR, &ledSettings);

	return 1;
}

int send_MIKE_ENDURANCE_TEST_START() {
	printf("\n#########################################################################################\n");
	printf("STARTING MIKE's ENDURANCE TEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("#########################################################################################\n\n");
	uint8_t ledColor = 0x01;
	uint8_t blinkingTimeInterval = 0x00;
	uint8_t blinkingTimeExpire = 0x00;
	uint8_t blinkingLedFlag = 0x00;

	uint8_t ledSettings[4] = {ledColor, blinkingTimeInterval, blinkingTimeExpire, blinkingLedFlag};

	memcpy(ledSettings, &ledColor, 1);
	memcpy(ledSettings + 1, &blinkingTimeInterval, 1);
	memcpy(ledSettings + 2, &blinkingTimeExpire, 1);
	memcpy(ledSettings + 3, &blinkingLedFlag, 1);

	send_SBLCP_message(SEND_SET_LED_TO_USER_DEFINED_COLOUR, &ledSettings);

	return 1;
}

int send_MIKE_ENDURANCE_TEST_STOP() {
	printf("\n#########################################################################################\n");
	printf("STOPPING MIKE's ENDURANCE TEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("#########################################################################################\n\n");

	uint8_t ledColor = 0x02;
	uint8_t blinkingTimeInterval = 0x00;
	uint8_t blinkingTimeExpire = 0x00;
	uint8_t blinkingLedFlag = 0x00;

	uint8_t ledSettings[4] = {ledColor, blinkingTimeInterval, blinkingTimeExpire, blinkingLedFlag};

	memcpy(ledSettings, &ledColor, 1);
	memcpy(ledSettings + 1, &blinkingTimeInterval, 1);
	memcpy(ledSettings + 2, &blinkingTimeExpire, 1);
	memcpy(ledSettings + 3, &blinkingLedFlag, 1);

	send_SBLCP_message(SEND_SET_LED_TO_USER_DEFINED_COLOUR, &ledSettings);

	return 1;
}

int send_SEND_GET_BREAKER_EOL_CALIB_PARAM_message() {
	g_manufacturingResetCount = 0;
	send_SBLCP_message(SEND_GET_BREAKER_EOL_CALIB_PARAM, NULL);

	return 1;
}

int send_SEND_SET_BREAKER_EOL_CALIB_PARAM_message() {
	// These are the default values from ADE9000.h
	g_manufacturingResetCount = 0;
	g_metroCalib.aphCal_0 	= ADE9000_APHCAL0;
	g_metroCalib.aiGain 	= ADE9000_AIGAIN;
	g_metroCalib.aiRmsOs 	= ADE9000_AIRMSOS;
	g_metroCalib.apGain 	= ADE9000_APGAIN;
	g_metroCalib.avGain 	= ADE9000_AVGAIN;
	g_metroCalib.bphCal_0 	= ADE9000_BPHCAL0;
	g_metroCalib.biGain 	= ADE9000_BIGAIN;
	g_metroCalib.biRmsOs 	= ADE9000_BIRMSOS;
	g_metroCalib.bpGain 	= ADE9000_BPGAIN;
	g_metroCalib.bvGain 	= ADE9000_BVGAIN;

	send_SBLCP_message(SEND_SET_BREAKER_EOL_CALIB_PARAM, &g_metroCalib);

	return 1;
}

int send_SEND_RESET_ENERGY_message() {
//	send_SBLCP_message(SEND_RESET_ENERGY, NULL);
	if((g_manufacturingResetCount == 0) || (g_manufacturingResetCount == 1))
	{
		send_SBLCP_message(SEND_RESET_ENERGY, NULL);
	}
	if(g_manufacturingResetCount == 2)
	{
		send_SBLCP_message(SET_MANUFACTURING_RESET, NULL);
		g_manufacturingResetCount = 0;
	}
	printf("\ng_manufacturingResetCount\n %d", g_manufacturingResetCount);
	g_manufacturingResetCount++;

	return 1;
}

uint16_t msg_len;
uint32_t start_byte;
uint32_t seq_number;
uint16_t msg_code;
char msg_data[200];
char msg_hash[200];

int stringExtractionFailedCounter = 0;

int send_customMessage() {
	int extractionSuccess = listenAndExtractCustomMessageParametersInString();

	if (extractionSuccess < 0) {
		// Error in string extraction
		ESP_LOGE("TAG", "udp_client_for_GUI.c: [SEFCMNS#{%d}] String extraction failed. Custom message not sent!", stringExtractionFailedCounter);

		++stringExtractionFailedCounter;

		return 0;
	}

	printf("[DEBUG] udp_client_for_GUI.c:send_customMessage(): Extraction success!\n");

	if (msg_code == SEND_SET_BREAKER_EOL_CALIB_PARAM) {
		printf("[DEBUG] udp_client_for_GUI.c:send_customMessage(): SEND_SET_BREAKER_EOL_CALIB_PARAM received!\n");

		extractCalibParamStringIntoStruct();

		return send_SBLCP_message(SEND_SET_BREAKER_EOL_CALIB_PARAM, &g_metroCalib);

	} else if (msg_code == SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER) {
		printf("[DEBUG] udp_client_for_GUI.c:send_customMessage(): SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER received!\n");

		uint32_t nextUDPSeqNum = extractSetNextUDPSequenceNumberStringIntoInt();

		uint32_t littleEndianVersionOfNextUDPSeqNum = bigEndianToLittleEndian32bits(nextUDPSeqNum);

		return send_SBLCP_message(SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER, &littleEndianVersionOfNextUDPSeqNum);

	}

	return 0;
}

/**
 * Returns -1 if fail, 0 if no fail, -2 if timeout
 */
int listenToBreakerResponseAndPrintResponse() {
	// ---------------------------------------------------------------------------
	// -----	Listen to response
	// ---------------------------------------------------------------------------
//	struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
	socklen_t socklen = sizeof(source_addr);

	const int MAX_TIME_OUT_AMOUNT = 100;

	// Set the timeout value (in this example, 3 seconds)
	struct timeval timeout;
//	timeout.tv_sec = 8;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	// Set up the file descriptors for the select function
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	// Wait for data to be received or timeout to occur
	int ready = select(sock + 1, &readfds, NULL, NULL, &timeout);

	if (ready == -1) {
		// Error occurred during select
		perror("select");
	} else if (ready == 0) {
		// Timeout occurred
		printf("[WARNING] udp_client_for_GUI.c: Timeout reached. No data received.\n\n");

		++timeOutCounter;
		if (timeOutCounter == MAX_TIME_OUT_AMOUNT) {
			printf("[WARNING] udp_client_for_GUI.c: Max timeout amount reached (%d). Restarting...\n", MAX_TIME_OUT_AMOUNT);

			vTaskDelay(pdMS_TO_TICKS(5000));	// Delay 5 sec

			// Restart if encounters 5 time outs.
			esp_restart();
		}

		return -2;
	} else {
		// Resets the timeOutCounter if successfully receive message.
		timeOutCounter = 0;

		int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

		printf("[DEBUG] udp_client_for_GUI.c: recvfrom-ed.\n");

		// Error occurred during receiving
		if (len < 0) {
			ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
			return -1;
		}
		// Data received
		else {
			inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
			rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string

//			if (!broadcasting) {
//				makeSureHostIPAddressIsRight(addr_str);
//			}

			printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>> Received Response <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
			ESP_LOGI(TAG, "Received %d bytes from %s on WiFi: %s.", len, addr_str, wifiSSID);

			start_msg = EXTRACT_DATA_4_BYTE(rx_buffer, 0);
			message_code = EXTRACT_DATA_2_BYTE(rx_buffer, 8);
			printf("received start_msg:-[%x]\n", start_msg);
			printf("received sequence number:-[%x]\n", EXTRACT_DATA_4_BYTE(rx_buffer, 4));
			printf("received message_code:-[%x]\n", message_code);

			if(message_code == GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER) {
				printf("\n");
				g_getSeqNum = (getSeqNumberMesg_st *)malloc(sizeof(getSeqNumberMesg_st));
				memcpy(g_getSeqNum, rx_buffer + HEADER_OFFSET, sizeof(getSeqNumberMesg_st));

				printf("Sequence Number:-[%x]\n", g_getSeqNum->seq_number);
				g_getSequenceNumber = g_getSeqNum->seq_number;
				printf("Serial Number:-[");
				for(uint8_t i = 0; i < 16; i++)
				{
					printf("%x,",g_getSeqNum->serial_number[i]);
				}
				printf("]\n");
				printf("Protocol Version:-[%x]\n", g_getSeqNum->version);
				printf("Nonce Value:-[%x]\n", g_getSeqNum->nonce);
				printf("Hash:-[");
				for(uint8_t i = 0; i < 32; i++)
				{
					printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(getSeqNumberMesg_st) + i]);
				}
				printf("]\n");

				// Make it also into JSON format:
				printf("GET_NEXT_UDP_JSON_START{");
				printf("\"Received Seq Num\": \"0x%x\",", g_getSeqNum->seq_number);
				printf(" \"Serial Number (hex)\": \"0x ");
				for(uint8_t i = 0; i < 16; i++) {
					printf("%x", g_getSeqNum->serial_number[i]);
					if (i != 15) printf(" ");	// Print a space after the number unless we are at the very end
				}
				printf("\",");
				printf(" \"Protocol Version\": %d,", g_getSeqNum->version);
				printf(" \"Nonce Value\": %d", g_getSeqNum->nonce);
				printf("}GET_NEXT_UDP_JSON_END\n");

				// Set breakerIdentifier_st stuff
				if (broadcasting) {
					// We are broadcasting
					// Check:
					// If we seen the ip before do something, just ignore it? TODO think about it
					// If we havent seen the ip before add to arrOfBreakerIdentifierStructs
					strcpy(arrOfBreakerIdentifierStructs[numberOfBreakersTalkedTo].ipAddress, addr_str);
					strcpy(arrOfBreakerIdentifierStructs[numberOfBreakersTalkedTo].serial_number, g_getSeqNum->serial_number);
					arrOfBreakerIdentifierStructs[numberOfBreakersTalkedTo].sequenceNumber = g_getSeqNum->seq_number;

					printf("[DEBUG] breakerIdentifier_st stuff: Index of: %d:\n", numberOfBreakersTalkedTo);
					printf("[DEBUG] ipAddress: %s,\n", arrOfBreakerIdentifierStructs[numberOfBreakersTalkedTo].ipAddress);
					printf("[DEBUG] serial_number: %s,\n", arrOfBreakerIdentifierStructs[numberOfBreakersTalkedTo].serial_number);
					printf("[DEBUG] sequenceNumber: 0x%08x\n", arrOfBreakerIdentifierStructs[numberOfBreakersTalkedTo].sequenceNumber);

					++numberOfBreakersTalkedTo;
				}

				free(g_getSeqNum);
			} else if ((message_code == GET_DEVICE_STATUS) || (message_code == GET_METER_TELEMETRY_DATA)) {	// TODO convert telem struct into device status struct
				printf("\n");

				printf("[INFO] Raw response: -[");
				for (size_t i = 0; i < len; i++) {
					printf("%02x", rx_buffer[i]);
					if (i != len - 1) printf(" ");
				}
				printf("]\n");

				if (message_code == GET_DEVICE_STATUS) {

					memcpy(g_metroData, rx_buffer + HEADER_OFFSET, sizeof(breakerStatus_st));
				} else {
					// message_code == GET_METER_TELEMETRY_DATA
					memcpy((uint8_t*)g_metroData + 3 * sizeof(uint8_t), rx_buffer + HEADER_OFFSET, sizeof(meterTelemetryData_st));
				}

//				uint8_t *tempForDebug = (uint8_t*) malloc(sizeof(breakerStatus_st));
//				memcpy(tempForDebug, rx_buffer + HEADER_OFFSET, sizeof(breakerStatus_st));
//				printf("[DEBUG] sizeof(breakerStatus_st) is: %d (Bytes)\n", sizeof(breakerStatus_st));
//				printf("[DEBUG] g_metroData: -[");
//				for (size_t i = 0; i < sizeof(breakerStatus_st); i++) {
//					printf("%02x", tempForDebug[i]);
//					if (i != sizeof(breakerStatus_st) - 1) printf(" ");
//				}
//				printf("]\n");
//				free(tempForDebug);

//				printf("breaker_state: [%u]\n", g_metroData->breaker_state);
//				printf("primary_handle_status: [%u]\n", g_metroData->primary_handle_status);
//				printf("meter_update_no: [%u]\n", g_metroData->telemetry_data.meter_update_no);
//				printf("period: [%u]\n", g_metroData->telemetry_data.period);
				printf("breaker_state: [0x%02x]\n", g_metroData->breaker_state);
				printf("primary_handle_status: [0x%02x]\n", g_metroData->primary_handle_status);
				printf("path_status: [0x%02x]\n", g_metroData->path_status);
				printf("meter_update_no: [0x%02x]\n", g_metroData->telemetry_data.meter_update_no);
				printf("period: [0x%04x]\n", g_metroData->telemetry_data.period);
				printf("phase_A_line_frequency: [%f]\n", g_metroData->telemetry_data.phase_A_line_frequency);
				printf("phase_A_rms_voltage: [%f]\n", g_metroData->telemetry_data.phase_A_rms_voltage);
				printf("phase_A_rms_current: [%f]\n", g_metroData->telemetry_data.phase_A_rms_current);
				printf("phase_A_power_factor: [%f]\n", g_metroData->telemetry_data.phase_A_power_factor);
				printf("phase_A_active_power: [%f]\n", g_metroData->telemetry_data.phase_A_active_power);
				printf("phase_A_reactive_power: [%f]\n", g_metroData->telemetry_data.phase_A_reactive_power);
				printf("phase_A_app_power: [%f]\n", g_metroData->telemetry_data.phase_A_app_power);
				printf("phase_A_active_energy: [%f]\n", g_metroData->telemetry_data.phase_A_active_energy);
				printf("phase_A_reactive_energy: [%f]\n", g_metroData->telemetry_data.phase_A_reactive_energy);
				printf("phase_A_app_energy: [%f]\n", g_metroData->telemetry_data.phase_A_app_energy);
				printf("phase_A_reverse_active_energy: [%f]\n", g_metroData->telemetry_data.phase_A_reverse_active_energy);
				printf("phase_A_reverse_reactive_energy: [%f]\n", g_metroData->telemetry_data.phase_A_reverse_reactive_energy);
				printf("phase_A_reverse_app_energy: [%f]\n", g_metroData->telemetry_data.phase_A_reverse_app_energy);
				printf("phase_B_line_frequency: [%f]\n", g_metroData->telemetry_data.phase_B_line_frequency);
				printf("phase_B_rms_voltage: [%f]\n", g_metroData->telemetry_data.phase_B_rms_voltage);
				printf("phase_B_rms_current: [%f]\n", g_metroData->telemetry_data.phase_B_rms_current);
				printf("phase_B_power_factor: [%f]\n", g_metroData->telemetry_data.phase_B_power_factor);
				printf("phase_B_active_power: [%f]\n", g_metroData->telemetry_data.phase_B_active_power);
				printf("phase_B_reactive_power: [%f]\n", g_metroData->telemetry_data.phase_B_reactive_power);
				printf("phase_B_app_power: [%f]\n", g_metroData->telemetry_data.phase_B_app_power);
				printf("phase_B_active_energy: [%f]\n", g_metroData->telemetry_data.phase_B_active_energy);
				printf("phase_B_reactive_energy: [%f]\n", g_metroData->telemetry_data.phase_B_reactive_energy);
				printf("phase_B_app_energy: [%f]\n", g_metroData->telemetry_data.phase_B_app_energy);
				printf("phase_B_reverse_active_energy: [%f]\n", g_metroData->telemetry_data.phase_B_reverse_active_energy);
				printf("phase_B_reverse_reactive_energy: [%f]\n", g_metroData->telemetry_data.phase_B_reverse_reactive_energy);
				printf("phase_B_reverse_app_energy: [%f]\n", g_metroData->telemetry_data.phase_B_reverse_app_energy);
				printf("phase_A_Q1_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q1_act_energy);
				printf("phase_A_Q2_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q2_act_energy);
				printf("phase_A_Q3_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q3_act_energy);
				printf("phase_A_Q4_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q4_act_energy);
				printf("phase_A_Q5_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q5_act_energy);
				printf("phase_A_Q1_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q1_reactive_energy);
				printf("phase_A_Q2_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q2_reactive_energy);
				printf("phase_A_Q3_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q3_reactive_energy);
				printf("phase_A_Q4_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q4_reactive_energy);
				printf("phase_A_Q5_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q5_reactive_energy);
				printf("phase_A_Q1_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q1_app_energy);
				printf("phase_A_Q2_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q2_app_energy);
				printf("phase_A_Q3_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q3_app_energy);
				printf("phase_A_Q4_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q4_app_energy);
				printf("phase_A_Q5_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_A_Q5_app_energy);
				printf("phase_B_Q1_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q1_act_energy);
				printf("phase_B_Q2_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q2_act_energy);
				printf("phase_B_Q3_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q3_act_energy);
				printf("phase_B_Q4_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q4_act_energy);
				printf("phase_B_Q5_act_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q5_act_energy);
				printf("phase_B_Q1_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q1_reactive_energy);
				printf("phase_B_Q2_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q2_reactive_energy);
				printf("phase_B_Q3_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q3_reactive_energy);
				printf("phase_B_Q4_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q4_reactive_energy);
				printf("phase_B_Q5_reactive_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q5_reactive_energy);
				printf("phase_B_Q1_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q1_app_energy);
				printf("phase_B_Q2_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q2_app_energy);
				printf("phase_B_Q3_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q3_app_energy);
				printf("phase_B_Q4_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q4_app_energy);
				printf("phase_B_Q5_app_energy: [%f]\n", (float)g_metroData->telemetry_data.phase_B_Q5_app_energy);
				printf("phase_to_phase_voltage: [%f]\n", g_metroData->telemetry_data.phase_to_phase_voltage);


				// Print JSON string
				printf("DEVICE_STATUS_JSON_START{");
				printf("\"breaker_state\": %u,", g_metroData->breaker_state);
				printf("\"primary_handle_status\": %u,", g_metroData->primary_handle_status);
				printf("\"path_status\": %u,", g_metroData->path_status);
				printf("\"meter_update_no\": %u,", g_metroData->telemetry_data.meter_update_no);
				printf("\"period\": %u,", g_metroData->telemetry_data.period);
				printf("\"phase_A_line_frequency\": %f,", g_metroData->telemetry_data.phase_A_line_frequency);
				printf("\"phase_A_rms_voltage\": %f,", g_metroData->telemetry_data.phase_A_rms_voltage);
				printf("\"phase_A_rms_current\": %f,", g_metroData->telemetry_data.phase_A_rms_current);
				printf("\"phase_A_power_factor\": %f,", g_metroData->telemetry_data.phase_A_power_factor);
				printf("\"phase_A_active_power\": %f,", g_metroData->telemetry_data.phase_A_active_power);
				printf("\"phase_A_reactive_power\": %f,", g_metroData->telemetry_data.phase_A_reactive_power);
				printf("\"phase_A_app_power\": %f,", g_metroData->telemetry_data.phase_A_app_power);
				printf("\"phase_A_active_energy\": %f,", g_metroData->telemetry_data.phase_A_active_energy);
				printf("\"phase_A_reactive_energy\": %f,", g_metroData->telemetry_data.phase_A_reactive_energy);
				printf("\"phase_A_app_energy\": %f,", g_metroData->telemetry_data.phase_A_app_energy);
				printf("\"phase_A_reverse_active_energy\": %f,", g_metroData->telemetry_data.phase_A_reverse_active_energy);
				printf("\"phase_A_reverse_reactive_energy\": %f,", g_metroData->telemetry_data.phase_A_reverse_reactive_energy);
				printf("\"phase_A_reverse_app_energy\": %f,", g_metroData->telemetry_data.phase_A_reverse_app_energy);
				printf("\"phase_B_line_frequency\": %f,", g_metroData->telemetry_data.phase_B_line_frequency);
				printf("\"phase_B_rms_voltage\": %f,", g_metroData->telemetry_data.phase_B_rms_voltage);
				printf("\"phase_B_rms_current\": %f,", g_metroData->telemetry_data.phase_B_rms_current);
				printf("\"phase_B_power_factor\": %f,", g_metroData->telemetry_data.phase_B_power_factor);
				printf("\"phase_B_active_power\": %f,", g_metroData->telemetry_data.phase_B_active_power);
				printf("\"phase_B_reactive_power\": %f,", g_metroData->telemetry_data.phase_B_reactive_power);
				printf("\"phase_B_app_power\": %f,", g_metroData->telemetry_data.phase_B_app_power);
				printf("\"phase_B_active_energy\": %f,", g_metroData->telemetry_data.phase_B_active_energy);
				printf("\"phase_B_reactive_energy\": %f,", g_metroData->telemetry_data.phase_B_reactive_energy);
				printf("\"phase_B_app_energy\": %f,", g_metroData->telemetry_data.phase_B_app_energy);
				printf("\"phase_B_reverse_active_energy\": %f,", g_metroData->telemetry_data.phase_B_reverse_active_energy);
				printf("\"phase_B_reverse_reactive_energy\": %f,", g_metroData->telemetry_data.phase_B_reverse_reactive_energy);
				printf("\"phase_B_reverse_app_energy\": %f,", g_metroData->telemetry_data.phase_B_reverse_app_energy);
				printf("\"phase_A_Q1_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q1_act_energy);
				printf("\"phase_A_Q2_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q2_act_energy);
				printf("\"phase_A_Q3_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q3_act_energy);
				printf("\"phase_A_Q4_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q4_act_energy);
				printf("\"phase_A_Q5_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q5_act_energy);
				printf("\"phase_A_Q1_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q1_reactive_energy);
				printf("\"phase_A_Q2_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q2_reactive_energy);
				printf("\"phase_A_Q3_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q3_reactive_energy);
				printf("\"phase_A_Q4_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q4_reactive_energy);
				printf("\"phase_A_Q5_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q5_reactive_energy);
				printf("\"phase_A_Q1_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q1_app_energy);
				printf("\"phase_A_Q2_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q2_app_energy);
				printf("\"phase_A_Q3_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q3_app_energy);
				printf("\"phase_A_Q4_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q4_app_energy);
				printf("\"phase_A_Q5_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_A_Q5_app_energy);
				printf("\"phase_B_Q1_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q1_act_energy);
				printf("\"phase_B_Q2_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q2_act_energy);
				printf("\"phase_B_Q3_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q3_act_energy);
				printf("\"phase_B_Q4_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q4_act_energy);
				printf("\"phase_B_Q5_act_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q5_act_energy);
				printf("\"phase_B_Q1_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q1_reactive_energy);
				printf("\"phase_B_Q2_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q2_reactive_energy);
				printf("\"phase_B_Q3_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q3_reactive_energy);
				printf("\"phase_B_Q4_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q4_reactive_energy);
				printf("\"phase_B_Q5_reactive_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q5_reactive_energy);
				printf("\"phase_B_Q1_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q1_app_energy);
				printf("\"phase_B_Q2_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q2_app_energy);
				printf("\"phase_B_Q3_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q3_app_energy);
				printf("\"phase_B_Q4_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q4_app_energy);
				printf("\"phase_B_Q5_app_energy\": %f,", (float)g_metroData->telemetry_data.phase_B_Q5_app_energy);
				printf("\"phase_to_phase_voltage\": %f", g_metroData->telemetry_data.phase_to_phase_voltage);
				printf("}DEVICE_STATUS_JSON_END\n");

				printf("Hash:-[");
				for(uint8_t i = 0; i < 32; i++)
				{
					printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(breakerStatus_st) + i]);
				}
				printf("]\n");
//				free(g_metroData);	// I comment this out because I want the g_metroData to maintain its values between messages
			} else if(	(message_code == GET_REMOTE_HANDLE_POSISTION) ||
				(message_code == SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER) ||
				(message_code == SET_LED_TO_USER_DEFINED_COLOUR) ||
				(message_code == SET_BREAKER_EOL_CALIB_PARAM) ||
				(message_code == SET_MANUFACTURING_MODE_ON) ||
				(message_code == SET_MANUFACTURING_MODE_OFF)) {
				g_response = (uint8_t *)malloc(sizeof(uint8_t));
				memcpy(g_response, rx_buffer + HEADER_OFFSET, sizeof(uint8_t));
				if(message_code == GET_REMOTE_HANDLE_POSISTION)
				{
					printf("\n");
					printf("HANDLE_POSITION:-[%d]HANDLE_POSITION_END\n", *g_response);
				}
				if(	(message_code == SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER) ||
					(message_code == SET_LED_TO_USER_DEFINED_COLOUR) ||
					(message_code == SET_BREAKER_EOL_CALIB_PARAM) ||
					(message_code == SET_MANUFACTURING_MODE_ON) ||
					(message_code == SET_MANUFACTURING_MODE_OFF))
				{
					printf("\n");
					printf("Response ACK/NACK:-[%d]\n", *g_response);
				}
				printf("Hash:-[");
				for(uint8_t i = 0; i < 32; i++)
				{
					printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(uint8_t) + i]);
				}
				printf("]\n");
				free(g_response);
			} else if (message_code == SET_REMOTE_HANDLE_POSISTION) {
				printf("\n");
				g_SetHandlePosistion = (setHandlePosition_st *)malloc(sizeof(setHandlePosition_st));
				memcpy(g_SetHandlePosistion, rx_buffer + HEADER_OFFSET, sizeof(setHandlePosition_st));
				printf("Response ACK/NACK:-[%d]\n", g_SetHandlePosistion->responce);
				printf("HANDLE_POSITION:-[%d]HANDLE_POSITION_END\n", g_SetHandlePosistion->handle_posistion);
				printf("Hash:-[");
				for(uint8_t i = 0; i < 32; i++)
				{
					printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(setHandlePosition_st) + i]);
				}
				printf("]\n");
				free(g_SetHandlePosistion);
			} else if (message_code == GET_BREAKER_EOL_CALIB_PARAM) {
				printf("\n");

				BreakerEOLCalibration_st *calibReceived;
				calibReceived = (BreakerEOLCalibration_st *)malloc(sizeof(BreakerEOLCalibration_st));
				memcpy(calibReceived, rx_buffer + HEADER_OFFSET, sizeof(BreakerEOLCalibration_st));

				printf("Phase_A cal 0:-[%d]\n", calibReceived->aphCal_0);
				printf("Phase_A Current Gain:-[%d]\n", calibReceived->aiGain);
				printf("Phase_A RMS current SoS:-[%d]\n", calibReceived->aiRmsOs);
				printf("Phase_A Power Gain:-[%d]\n",calibReceived->apGain);
				printf("Phase_A voltage Gain:-[%d]\n", calibReceived->avGain);
				printf("Phase_B cal 0:-[%d]\n", calibReceived->bphCal_0);
				printf("Phase_B Current Gain:-[%d]\n", calibReceived->biGain);
				printf("Phase_B RMS current SoS:-[%d]\n", calibReceived->biRmsOs);
				printf("Phase_B Power Gain:-[%d]\n",calibReceived->bpGain);
				printf("Phase_B voltage Gain:-[%d]\n", calibReceived->bvGain);

				printf("CALIB_PARAM_JSON_START{");
				printf("\"aphCal_0\": %d,", calibReceived->aphCal_0);
				printf(" \"aiGain\": %d,", calibReceived->aiGain);
				printf(" \"aiRmsOs\": %d,", calibReceived->aiRmsOs);
				printf(" \"apGain\": %d,",calibReceived->apGain);
				printf(" \"avGain\": %d,", calibReceived->avGain);
				printf(" \"bphCal_0\": %d,", calibReceived->bphCal_0);
				printf(" \"biGain\": %d,", calibReceived->biGain);
				printf(" \"biRmsOs\": %d,", calibReceived->biRmsOs);
				printf(" \"bpGain\": %d,",calibReceived->bpGain);
				printf(" \"bvGain\": %d", calibReceived->bvGain);
				printf("}CALIB_PARAM_JSON_END\n");

				printf("Hash:-[");
				for(uint8_t i = 0; i < 32; i++)
				{
					printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(BreakerEOLCalibration_st) + i]);
				}
				printf("]\n");
				free(calibReceived);
			} else if (message_code == RESET_ENERGY) {
				printf("\n");

				uint8_t *ackNack = malloc(sizeof(uint8_t));
				memcpy(ackNack, rx_buffer + HEADER_OFFSET, sizeof(uint8_t));

				printf("Response ACK/NACK:-[%d]\n", *ackNack);

				free(ackNack);
			} else if (message_code == GET_PRIMARY_HANDLE_STATUS) {
				// TODO after James implement his stuff
			} else if (message_code == GET_TRIP_CURVE) {
				printf("\n");

ErrorLog_st *trip_log = malloc(sizeof(ErrorLog_st));
                // char errorLog[TRIP_LOG_SIZE];
                // char epochTime[TRIP_LOG_SIZE];
                // char cloudUpdate[TRIP_LOG_SIZE];
 
                memcpy(trip_log, rx_buffer + HEADER_OFFSET, sizeof(ErrorLog_st));
 
                // for(uint16_t i = 0; i < TRIP_LOG_SIZE; i++)
                // {
                //  errorLog[i] = trip_log->logs[i].ErrorLog;
                //  epochTime[i] = trip_log->logs[i].EpochTime;
                //  cloudUpdate[i] = trip_log->logs[i].CloudUpdated;
                // }
 
//              printf("TRIP_PARAM_JSON_START{");
//              printf("\"errorLog\": %s", errorLog);
//              printf("\"epochTime\": %s", epochTime);
//              printf("\"cloudUpdate\": %s", cloudUpdate);
//              printf("}TRIP_PARAM_JSON_END\n");
 
 
                printf("TRIP_PARAM_JSON_START{");
                for(uint16_t i = 0; i < TRIP_LOG_SIZE; i++)
                {
					if (trip_log->logs[i].EpochTime!=0)
                    {
					time_t default_time = trip_log->logs[i].EpochTime;
                    //struct tm ts = *localtime(&default_time);
                    char buf[80];
                    strftime(buf, sizeof(buf), "%a--%Y-%m-%d--%H-%M-%S", localtime(&default_time));
					
                    //if (i == (TRIP_LOG_SIZE - 1)) {
                      //  printf("%d-%s:Hex-%x-Dec-%d",i,buf, trip_log->logs[i].ErrorLog,trip_log->logs[i].ErrorLog);
                    //} else {
                        printf("%d-%s:Hex-%x-Dec-%d,",i,buf,trip_log->logs[i].ErrorLog,trip_log->logs[i].ErrorLog);
                    //}
					}
 
                }
				printf("End:End");
                printf("}TRIP_PARAM_JSON_END\n");
 
 
//              for(uint16_t i = 0; i < TRIP_LOG_SIZE; i++)
//              {
//                  printf("[%x],[%d],[%d]\n", trip_log->logs[i].ErrorLog, trip_log->logs[i].EpochTime, trip_log->logs[i].CloudUpdated);
//              }
//              printf("Index %d\n", trip_log->index);
 
                printf("Hash:-[");
                for(uint8_t i = 0; i < 32; i++)
                {
                    printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(ErrorLog_st) + i]);
                }
                printf("]\n");
                free(trip_log);
			} else if (message_code == GET_MAC_ADDRESS) {
				printf("\n");

				uint8_t addr[20];

				memcpy(addr, rx_buffer + HEADER_OFFSET, sizeof(addr));

//				printf("MacAddress:-[%s]\n", addr);

				printf("MAC_ADDRESS_JSON_START{");
				printf("\"MacAddress\": %s", addr);
				printf("}MAC_ADDRESS_JSON_END\n");

				printf("Hash:-[");
				for(uint8_t i = 0; i < 32; i++)
				{
					printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(BreakerMacAddress_st) + i]);
				}
				printf("]\n");
			} else if (message_code == SET_MANUFACTURING_RESET) {
			 	printf("\n");

				uint8_t *ackNack = malloc(sizeof(uint8_t));
				memcpy(ackNack, rx_buffer + HEADER_OFFSET, sizeof(uint8_t));

				printf("Response ACK/NACK:-[%d]\n", *ackNack);


				printf("Hash:-[");
				for(uint8_t i = 0; i < 32; i++)
				{
					printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(BreakerMacAddress_st) + i]);
				}
				printf("]\n");
				free(ackNack);
			} else if (message_code == SET_DEVICE_LOCK) {
				printf("\n");

				uint8_t *ackNack = malloc(sizeof(uint8_t));
				memcpy(ackNack, rx_buffer + HEADER_OFFSET, sizeof(uint8_t));

				printf("Response ACK/NACK:-[%d]\n", *ackNack);


				printf("Hash:-[");
				for(uint8_t i = 0; i < 32; i++)
				{
					printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(BreakerMacAddress_st) + i]);
				}
				printf("]\n");
				free(ackNack);
			}else if (message_code == IDENTIFY_ME) {
			printf("\n");

			uint8_t *ackNack = malloc(sizeof(uint8_t));
			memcpy(ackNack, rx_buffer + HEADER_OFFSET, sizeof(uint8_t));

			printf("Response ACK/NACK:-[%d]\n", *ackNack);


			printf("Hash:-[");
			for(uint8_t i = 0; i < 32; i++)
			{
				printf("%x ",rx_buffer[HEADER_OFFSET + sizeof(BreakerMacAddress_st) + i]);
			}
			printf("]\n");
			free(ackNack);
			} else {
				// hello
			}

			printf("[INFO] Raw response: -[");
			for (size_t i = 0; i < len; i++) {
				printf("%02x", rx_buffer[i]);
				if (i != len - 1) printf(" ");
			}
			printf("]\n");

			++response_counter;	// The first response will ge response_counter == 1
			printf("[INFO] This is the #<%d> response we got so far.\n", response_counter);

			printf("========================= Respond End =========================\n\n");

			// Increment seq number so that next time we send a message it is seqnumber + 1
			g_getSequenceNumber += 1;

			return 0;
		}
	}
	return 0;	// Default
}

/**
 * This is for sending custom messages. When 'z' is pressed, a string that contains the info for
 * the custom string will follow. This function's job is to listen to that string and extract the
 * info from that string.
 */
int listenAndExtractCustomMessageParametersInString() {
	ESP_LOGI(TAG, "Custom key pressed!");

	char message_string[200] = "";

	// Grab the data and save it in message_string
	char current;
	while (current != '\n') {
		scanf("%c", &current);
		strncat(message_string, &current, 1);
	}

	printf("[INFO] udp_client_for_GUI.c: Received: %s\n", message_string);

	// v This is such bad memory management I dont even want to talk about it
	char junk[20];
	char startByte[20];
	char sequenceNumber[20];
	char messageCode[20];
	char messageData[200];
	char messageHash[200];

	// Extract the tokens from the message_string and store then in a string first
	char* stringToken = strtok(message_string, ",");
	int i = 0;
	while (stringToken != NULL) {
		switch (i) {
			case 0:
				strcpy(junk, stringToken);
				break;
			case 1:
				strcpy(startByte, stringToken);
				break;
			case 2:
				strcpy(sequenceNumber, stringToken);
				break;
			case 3:
				strcpy(messageCode, stringToken);
				break;
			case 4:
				strcpy(messageData, stringToken);
				break;
			case 5:
				strcpy(messageHash, stringToken);
				break;
			default:
				// Something is wrong, maybe a duplicated ',' is sent.
				ESP_LOGE("TAG", "Incorrect string format detected!");
				break;
		}

		++i;
		// Grab next token
		stringToken = strtok(NULL, ",");
	}

	// Save them into variables for send_customMessage() to use
	start_byte = STAR_OF_MESSAGE;	// Since all start byte is the same
	seq_number = hexStringToUint32(sequenceNumber);
	msg_code = hexStringToUint16(messageCode);
	strcpy(msg_data, messageData);
	strcpy(msg_hash, messageHash);

	int verify = verifyStingRead();

	// TODO           vvv these comparison might be an issue cus they are unsigned
//	if (verify < 0 || seq_number < 0 || msg_code < 0) {
	if (verify < 0 || seq_number & 0x80000000 || msg_code & 0x8000) { // To test
		// If any of the verification failed
		return -1;
	}

	printf("[INFO] udp_client_for_GUI.c:listenAndExtractCustomMessageParametersInString(): Custom message string extraction is a success!\n");

	return 0;
}

/**
 * Verifies the string read (pass tense), the idea is to detect corrupted string
 * and signals to Java code something is wrong.
 */
int verifyStingRead() {
	int toReturn = 0;
	// Check if we got a valid message code
	if (msg_code == SEND_GET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER ||
		msg_code == SEND_GET_DEVICE_STATUS ||
		msg_code == SEND_GET_REMOTE_HANDLE_POSISTION ||
		msg_code == SEND_GET_METER_TELEMETRT_DATA ||
		msg_code == SEND_SET_NEXT_EXPECTED_UDP_SEQUENCE_NUMBER ||
		msg_code == SEND_SET_REMOTE_HANDLE_POSISTION ||
		msg_code == SEND_SET_LED_TO_USER_DEFINED_COLOUR ||
		msg_code == SEND_SET_BREAKER_EOL_CALIB_PARAM ||
		msg_code == SEND_GET_BREAKER_EOL_CALIB_PARAM ||
		msg_code == SEND_SET_MANUFACTURING_MODE_ON ||
		msg_code == SEND_SET_MANUFACTURING_MODE_OFF ||
		msg_code == SEND_RESET_ENERGY ||
		msg_code == SEND_GET_PRIMARY_HANDLE_STATUS ||
		msg_code == SEND_GET_METER_TELEMETRY_DATA || 
		msg_code == SEND_GET_TRIP_CURVE ||
		msg_code == SEND_GET_MAC_ADDRESS)
	{
		printf("[INFO] udp_client_for_GUI.c:verifyStingRead(): Valid message code (0x%x) detected.\n", msg_code);
	} else {
		printf("[INFO] udp_client_for_GUI.c:verifyStingRead(): Valid message code NOT detected, receive corrupted string (0x%x).\n", msg_code);
		toReturn = -1;
	}

	// TODO ...
	return toReturn;
}

/**
 * This function's main job is to make sure we only use the broadcast IP address (255.255.255.255)
 * once (when sending out handshake).
 */
void makeSureHostIPAddressIsRight(char* givenIPAddress) {
	// currentHostAddressIsNotBroadcastIP == 0 (false) if HOST_IP_ADDR is 255.255.255.255
	// currentHostAddressIsNotBroadcastIP != 0 (true) if HOST_IP_ADDR is NOT 255.255.255.255
	int currentHostAddressIsNotBroadcastIP = strcmp(HOST_IP_ADDR, "255.255.255.255");
	if (!currentHostAddressIsNotBroadcastIP) {
		// Log
		ESP_LOGI(TAG, "Updating host ip to: %s, was: %s", givenIPAddress, HOST_IP_ADDR);
		// current host address is 255.255.255.255
		// which means we are just sending out handshake
		// which means we want to set givenIPAddress as our new host IP:
		HOST_IP_ADDR = givenIPAddress;
	} else if (currentHostAddressIsNotBroadcastIP) {
		// current host address is not 255.255.255.255
		// which means we sent out handshake and now we are listening to other messages
		// which means if we are connected to the same device our ip shouldnt change
		int theHostAddressDidChange = strcmp(HOST_IP_ADDR, givenIPAddress);
		// theHostAddressDidChange == 0 (false) host ip did NOT change
		// theHostAddressDidChange != 0 (true) host ip did change
		if (theHostAddressDidChange) {
			// The host address changed, which is weird
//			ESP_LOGI(TAG, "Very weird the host ip is %s", HOST_IP_ADDR);
			// TODO do something about it
		} else {
			// The host address did not change, which is normal
//			ESP_LOGI(TAG, "Very normal the host ip is %s", HOST_IP_ADDR);
			return;	// Done, leave the function
		}
	}
}

uint8_t hexStringToUint8(char* givenString) {
	char* errorPointer;
	uint8_t toReturn = (uint8_t)strtoul(givenString, &errorPointer, 16);

	if (*errorPointer != '\0') {
		// if *errorPointer != '\0' aka input string does not represent a valid integer
		printf("[ERROR] udp_client_for_GUI.c:hexStringToUint8(): Invalid string input of: %s.\n",
				givenString);
		return -1;
	}

	return toReturn;
}

uint16_t stringToUint16(char* givenString) {
	uint16_t toReturn = atoi(givenString);

	if (!toReturn) {
		// if toReturn == 0 aka input string does not represent a valid integer
		printf("[ERROR] udp_client_for_GUI.c:stringToUint16(): Invalid string input of: %s.\n", givenString);
		toReturn = 0;
	}

	return toReturn;
}

uint16_t hexStringToUint16(char* givenString) {
	char* errorPointer;
	uint16_t toReturn = (uint16_t)strtoul(givenString, &errorPointer, 16);

	if (*errorPointer != '\0') {
		// if *errorPointer != '\0' aka input string does not represent a valid integer
		printf("[ERROR] udp_client_for_GUI.c:hexStringToUint16(): Invalid string input of: %s.\n",
				givenString);
		return -1;
	}

	return toReturn;
}

uint32_t stringToUint32(char* givenString) {
	uint32_t toReturn = atoi(givenString);

	if (!toReturn) {
		// if toReturn == 0 aka input string does not represent a valid integer
		printf("[ERROR] udp_client_for_GUI.c:stringToUint32(): Invalid string input of: %s.\n", givenString);
		toReturn = 0;
	}

	return toReturn;
}

uint32_t hexStringToUint32(char* givenString) {
	char* errorPointer;
	uint32_t toReturn = (uint32_t)strtoul(givenString, &errorPointer, 16);

	if (*errorPointer != '\0') {
		// if *errorPointer != '\0' aka input string does not represent a valid integer
		printf("[ERROR] udp_client_for_GUI.c:hexStringToUint32(): Invalid string input of: %s.\n",
				givenString);
		return -1;
	}

	return toReturn;
}

/**
 * A SET_BREAKER_EOL_CALIB_PARAM message have parameters separated with ';',
 * for example "123;321;456;654;789;987;007;700;505;626;115", so the code below
 * is grabbing each parameter and storing them into the g_metroCalib struct.
 * Note: I assume that the string is not corrupted and there are 10 parameters
 * 		 to be extracted.
 */
//#define DEBUGGING_extractCalibParamStringIntoStruct_function
int extractCalibParamStringIntoStruct() {
	char* msgDataToken = strtok(msg_data, ";");
	int count = 0;
	while (msgDataToken != NULL) {
		switch (count) {
			case 0:
				g_metroCalib.aphCal_0 = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c: g_metroCalib.aphCal_0 is set to: %d\n", g_metroCalib.aphCal_0);
#endif
				break;
			case 1:
				g_metroCalib.aiGain = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c: g_metroCalib.aiGain is set to: %d\n", g_metroCalib.aiGain);
#endif
				break;
			case 2:
				g_metroCalib.aiRmsOs = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c:  g_metroCalib.aiRmsOs is set to: %d\n", g_metroCalib.aiRmsOs);
#endif
				break;
			case 3:
				g_metroCalib.apGain = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c: g_metroCalib.apGain is set to: %d\n", g_metroCalib.apGain);
#endif
				break;
			case 4:
				g_metroCalib.avGain = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c: g_metroCalib.avGain is set to: %d\n", g_metroCalib.avGain);
#endif
				break;
			case 5:
				g_metroCalib.bphCal_0 = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c:  g_metroCalib.bphCal_0 is set to: %d\n", g_metroCalib.bphCal_0);
#endif
				break;
			case 6:
				g_metroCalib.biGain = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c: g_metroCalib.biGain is set to: %d\n", g_metroCalib.biGain);
#endif
				break;
			case 7:
				g_metroCalib.biRmsOs = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c:  g_metroCalib.biRmsOs is set to: %d\n", g_metroCalib.biRmsOs);
#endif
				break;
			case 8:
				g_metroCalib.bpGain = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c: g_metroCalib.bpGain is set to: %d\n", g_metroCalib.bpGain);
#endif
				break;
			case 9:
				g_metroCalib.bvGain = (uint32_t)atoi(msgDataToken);
#ifdef DEBUGGING_extractCalibParamStringIntoStruct_function
				printf("[DEBUG] udp_client_for_GUI.c: g_metroCalib.bvGain is set to: %d\n", (g_metroCalib.bvGain));
#endif
				break;
			default:
				break;
		}

		++count;
		msgDataToken = strtok(NULL, ";");
	}

	return 0;
}

//#define DEBUGGING_extractSetNextUDPSequenceNumberStringIntoInt_FUNCTION
uint32_t extractSetNextUDPSequenceNumberStringIntoInt() {
	uint32_t toReturn;

	// Turn msg_data (string) into a uint8_t
	toReturn = hexStringToUint32(msg_data);

#ifdef DEBUGGING_extractSetNextUDPSequenceNumberStringIntoInt_FUNCTION
	printf("[DEBUG] udp_client_for_GUI.c:extractSetNextUDPSequenceNumberStringIntoInt(): Inputed UDP Sequence number to set: 0x%x\n", toReturn);
	printf("[DEBUG] udp_client_for_GUI.c:extractSetNextUDPSequenceNumberStringIntoInt(): Inputed UDP Sequence number to set: %d\n", toReturn);
#endif

	return toReturn;
}

uint32_t bigEndianToLittleEndian32bits(uint32_t num) {
	uint32_t result = ((num >> 24) 	& 0xFF			) |
					  ((num >> 8) 	& 0xFF00		) |
					  ((num << 8) 	& 0xFF0000		) |
					  ((num << 24) 	& 0xFF000000	);
	return result;
}
