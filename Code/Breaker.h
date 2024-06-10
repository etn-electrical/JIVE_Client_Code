/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : Breaker.h
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/19/2022
 *
 ****************************************************************************/

/****************************************************************************
 *                              INCLUDE FILES                               
 ****************************************************************************/





/****************************************************************************
 *                              General definitions                               
 ****************************************************************************/




#ifndef __Breaker__H
#define __Breaker__H

#include <stdio.h>
#include "string.h"
#include "driver/mcpwm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "hal/timer_hal.h"

#include "LED_Breaker.h"
#include "Flash.h"
//#include "Uart_events.h"
#include "Metrology/metro.h"
#include "EERAM/eeram_comp.h"

#define MY_FIRMWARE_VERSION    0x240429  //Has to be YYMMDD

//#define _FACTORY_BUILD_ // Don't forget to enable _DISABLE_DEBUG_MESSAGES_ in Debug_Manager_config.h

//#define _AUTOMATIC_PROVISIONIG_NO_BUTTONS_OR_RESET_
//#define _OLD_APP_NO_BREAKER_
//#define _TRACK_HEAP_SIZE_
//#define _DEVICE_HAS_PASSWORD_


//#define _DELETE_CONNECTION_STRING_TO_TEST_DPS_
//#define _KEEP_REPORVISINING_EVERY_TIME_POWERING_UP_
//#define _AHMED_PROVISION_TEST_
//#define _AHMED_ADD_CONNECTION_STRING_

//#define _RESET_AFTER_CONNECTION_
//#define _ERROR_LOG_DEBUG_

//#define _DISABLE_METROLOGY_
//#define _DISABLE_M2M_COMMUNICATION_DURING_PROVISIONING_

#ifdef _FACTORY_BUILD_
    #undef _OLD_APP_NO_BREAKER_
    #undef _AUTOMATIC_PROVISIONIG_NO_BUTTONS_OR_RESET_
    #undef _DELETE_CONNECTION_STRING_TO_TEST_DPS_
    #undef _TRACK_HEAP_SIZE_
    #define _DISABLE_METROLOGY_
    #define _DISABLE_SBLCP_
    #undef _KEEP_REPORVISINING_EVERY_TIME_POWERING_UP_
    #undef _DISABLE_M2M_COMMUNICATION_DURING_PROVISIONING_

#endif

//#define _DEBUG_

#define DEVICE_NAME_DEFAULT             "SmartBreaker-2.0" // Don't change the name here and in the excel sheet, that may break OTA

#define CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
//#define CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
    #undef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
#endif
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
    #undef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
#endif

#ifdef _ERROR_LOG_DEBUG_

    #define LOG_BREAKER_TURN_ON             0x70
    #define LOG_BREAKER_TURN_OFF            0x71
    #define LOG_BREAKER_PROVISIONING        0x72
    #define LOG_BREAKER_RE_PROVISIONING     0x73
    #define LOG_BREAKER_TIMER_BASED_LOG     0x74

#endif



#define ON                      0x01
#define OFF                     0x00

#define RISING_EDGE_SIGNAL      0x01
#define FALLING_EDGE_SIGNAL     0x00

#define HI                      0x01
#define LOW                     0x00




#define DEVICE_IS_NOT_IN_A_NETWORK   0x5A
#define DEVICE_IS_IN_A_NETWORK       0xA5

#define RE_START_PROVISIONING        0xAA

#define FACTORY_LOCATION_1 0x01
#define FACTORY_LOCATION_2 0x02

#define INVALID_VALUE           0xF0

#define ERROR_LOG_INITIATED		0x5A

#define ConnectionStringLength 	170
#define HostNameLength 			55
#define DeviceIdLength 			45
#define SharedAccessKeyLength 	50
#define ConnectionStringMinValidLength  100

#define DpsConcatenatedMessageLength 237 // see ParseDPSConcatenatedMessage, CreateDPSConcatenatedMessage functions for size guidance
#define DpsEndpointLength       80
#define DpsIdScopeLength        20
#define DpsSymmetricKeyLength   45
#define DpsDeviceRegIdLength    37

#define DpsConcatenatedMessageMinValidLength 85

#define MAX_SIZE_DEVICE_NAME_DEFAULT 32

/****************************************************************************
 *                              Reseting Types     
 ****************************************************************************/
 #define MANUFACTURING_RESET        0x00
 #define PROVISIONING_RESET         0x01
 #define DEVICE_RESET               0x04 //restart the device, keeping everythign in flash as it is
 
 #define DEVICE_RESET_STATE_OFF     0x00
 #define DEVICE_RESET_STATE_ON      0xA5
 
 #define DEVICE_IS_IN_FACTORY_MODE  0xA5 
 
 #define DEVICE_IS_LOCKED           0xAA

 #define DEVICE_IS_NOT_LOCKED       0xA6
 
 #define BUILD_TYPE_SAVED           0x5A

 #define BUILD_TYPE_NOT_SAVED       0x55
 
 #define NEW_PROGRAMMED_PART_NO_SAVED    0x5B

 #define NEW_PROGRAMMED_PART_NO_NOT_SAVED     0x5C

/****************************************************************************
 *                              Default values     
 ****************************************************************************/
#define MAGIC_VALUE					0xAE
#define PROD_NVS_INIT_DONE			0X5A


/****************************************************************************
 *                              Timers definitions                               
 ****************************************************************************/
 


#define TEN_MS                  10  //base on 1ms timer

#define ONE_SECOND              100 // based on 10ms timer
#define ONE_AND_HALF_SECOND     150 // based on 10ms timer
#define TWO_SECOND_MS           200 // based on 10ms timer
#define THREE_SECOND_MS         300 // based on 10ms timer
#define FIVE_SECOND_MS          500 // based on 10ms timer
#define FIFTY_MS                5   // based on 10 ms timer
#define ONE_HUNDERED_MS         10  // based on 10 ms timer
#define TWO_HUNDRED_MS          20  // based on 10ms timer
#define FOUR_HUNDRED_MS         40  // based on 10ms timer
#define FIVE_HUNDRED_MS         50  // based on 10ms timer
#define SEVEM_HUNDRED_MS        70  // based on 10ms timer
#define TWINTY_SECONDS_10ms     2000  // based on 10ms timer


//Timers

#define HUNDRED_MS_TIMER            2  //based on 50ms timer
#define QUARTER_SECOND_TIMER        5  //based on 50ms timer
#define HALF_SECOND_TIMER           10  //based on 50ms timer
#define THREE_QUARTER_SECOND_TIMER  15  //based on 50ms timer
#define ONE_SECOND_TIMER            20  //based on 50ms timer
#define ONE_SECOND_AND_HALF_TIMER   30  //based on 50ms timer
#define TWO_SECOND_TIMER            40  //based on 50ms timer
#define THREE_SECOND_TIMER          60  //based on 50ms timer
#define FIVE_SECOND_TIMER           100 //based on 50ms timer
#define TEN_SECOND_TIMER            200 //based on 50ms timer


#define WIFI_CONNECT_WAIT_TIMER             ONE_SECOND_AND_HALF_TIMER
#define WRITING_TO_FLASH_WAIT_TIMER         TWO_SECOND_TIMER




#define TWO_SECONDS                     2       // based on 1 second timer  
#define THREE_SECONDS                   3       // based on 1 second timer  
#define FIVE_SECONDS                    5       // based on 1 second timer  
#define SEVEN_SECONDS                   7       // based on 1 second timer  
#define TEN_SECONDS                     10      // based on 1 second timer
#define FIFTEN_SECONDS                  15      // based on 1 second timer
#define TWINTY_SECONDS                  20      // based on 1 second timer
#define THIRTY_SECONDS                  30      // based on 1 second timer
#define FOURTY_SECONDS                  40      // based on 1 second timer
#define ONE_MINUTE                      60      // based on 1 second timer
#define TWO_MINUTES                     120     // based on 1 second timer
#define THREE_MINUTES                   180     // based on 1 second timer
#define FIVE_MINUTES                    300     // based on 1 second timer
#define FIFTEN_MINUTES                  900     // based on 1 second timer
#define TWINTY_MINUTES                  1200    // based on 1 second timer
#define THIRTY_MINUTES                  1800    // based on 1 second timer
#define ONE_HOUR                        3600    // based on 1 second timer
#define TWO_HOURS                       7200    // based on 1 second timer

#define ONE_HOUR_M                      60    // based on 1 minutetimer
#define ONE_DAY                         24    // based on 1 hour timer

#define CRYPTO_HASH_LENGTH			32
#define SERIAL_NUMBER_LENGTH		16
#define CATALOG_LENGTH				12
#define STYLE_NUMBER_LENGTH			12
#define	PCB_SERIAL_NUMBER_LENGTH	12
#define FIRMWARE_VER_LENGTH			8

//#define _DEBUG_TEMP_

// Breaker state definition
typedef enum
{
    LOAD_OFF =   0,  
    LOAD_ON,
    LOAD_UNKNOWN,
 
}Load_State_enum;

typedef enum
{
    SW_UNKNOWN = 0,
    SW_OPEN,
    SW_TRIP,
    SW_CLOSED,
    SW_FAILED,
} pswitch_status_t;

typedef enum
{
	SS_UNKNOWN = 0,
    SS_OPEN,
	SS_OPENING,
    SS_CLOSED,
	SS_CLOSING,
    SS_STUCK_OPEN,
	SS_STUCK_CLOSED,
	#if defined INCLUDE_T_WELD
	SS_TRY_T_WELD, //try using t_weld to open
	#endif

} ss_status_t;

typedef enum
{
	PATH_UNKNOWN = 0, //may be transitioning between states.
    PATH_OPEN,
    PATH_CLOSED,
	PATH_1P_STUCK,

} fdbk_status_t;



typedef enum
{
    UnProvisioned =   0, // That doesn't mean the device is not provisioned, it means we are not in the provision statemachine
    ProvisionStarted,
    ProvisionGotIP,
    ProvisionGotSSID,
    NotProvisionedNotConnected,
    ProvisionedNotConnected,
    ProvisionWrongPassword,
    Provisioned,
    Provision_Save_SSID,
    Provision_Save_PassWord,
    Provision_Save_ConnectionString,
    ProvisionSavedInfo
}Provision_State_enum;


typedef enum
{
    OTA_NON =   0,
    OTA_START,
    OTA_MEM_EREASE,
    OTA_MEM_WRITE,
    OTA_BOOT_WRITE,
}OTA_State_enum;


typedef struct
{
	pswitch_status_t  PrimaryContactState;
	ss_status_t       SecondaryContactState;
	fdbk_status_t     PathStatus;
    uint8_t    DeviceInNetworkStatus; // DEVICE_IS_NOT_IN_A_NETWORK or DEVICE_IS_IN_A_NETWORK
    uint8_t    WiFiDisconnected; 	    // This flag indicate the device is provisioned and connected to IoT hub, but we lost WiFi connection
    //Wifi connection
    bool       WiFiConnected; // Have an IP address
    uint8_t    WiFiConnectedWaitTimer; // Time to wait before starting PxGreen
    bool       IoTConnected;  // Connected to Eaton Cloud
    bool       ConnectionStringSaved;  //To indicate if we have a connection string programmed
    bool       PxInitiated; // Initiated Px green and DCI

    //IoT
    char ConnectionString[ConnectionStringLength];
    char HostName[HostNameLength];
    char DeviceId[DeviceIdLength];
    char SharedAccessKey[SharedAccessKeyLength];
    Provision_State_enum    ProvisionState;
    char Ssid[32];
    char PassWord[32];
    uint8_t MacAddress[20];
    
    OTA_State_enum OTA_State;
    char DeviceName[32];
    bool DeviceNameChanged;
   
    uint8_t DeviceInFactoryMode;
    bool DidWeCheckTheConnectionString;
    uint8_t TheDeviceIsLocked;
    bool DeinitProvisioningStart;
    
    uint8_t BuildTypeSaved;
    char BuildType[32];

    uint8_t NewProgrammedPartNumberSaved;
    char NewProgramedPartNumber[32];
    //bool JustProvisioned; //this is to help not to re-initialize BLE
    uint8_t StartReProvisioning;

    //DPS for IoT
    char DpsConcatenatedMessage[DpsConcatenatedMessageLength];
    char DpsEndpoint[DpsEndpointLength];
    char DpsIdScope[DpsIdScopeLength];
    char DpsSymmetricKey[DpsSymmetricKeyLength];
    char DpsDeviceRegId[DpsDeviceRegIdLength];
    
    bool DpsInfoSaved;                     // Do we have info programmed to connect to DPS
    bool DpsReturnedConnectionStringSaved; // Have we successfully registered with DPS and saved a new IOT connection string
    bool ConnectionStringSaveNeeded;
    bool DpsConfigSaveNeeded;

    uint8_t M2MUartStatus;

    unsigned char UnicastPrimaryUDPKey[CRYPTO_HASH_LENGTH + 1];
    unsigned char UnicastSecondaryUDPKey[CRYPTO_HASH_LENGTH + 1];
    unsigned char BroadcastPrimaryUDPKey[CRYPTO_HASH_LENGTH + 1];
    unsigned char BroadcastSecondaryUDPKey[CRYPTO_HASH_LENGTH + 1];

    bool bSblcpInit;
   
    bool MetroInitStatus;

	uint8_t current_rating;
	uint8_t PCB_hardware_ver;
	char catalog_number[CATALOG_LENGTH + 1];
	char style_number[STYLE_NUMBER_LENGTH + 1];
	char breaker_serial_no[SERIAL_NUMBER_LENGTH + 1];
	char ESP_firmware_ver[FIRMWARE_VER_LENGTH + 1];
	char STM_firmware_ver[FIRMWARE_VER_LENGTH + 1];
	char PCB_serial_number[PCB_SERIAL_NUMBER_LENGTH + 1];
	bool breaker_test_status;
	bool PCBA_test_status;
    bool EERAMInitStatus;
    bool Protection_FW_Version_Read;
    int8_t Wifi_RSSI_Signal_Strength;
    bool Sblcp_En_Dis;
    bool Metro_Ade_Spi_Error;
    uint8_t Device_Factory_Location;
    uint8_t Breaker_Fault_State;
} Device_Info_str;

#define RING_BUFFER_ARRAY_SIZE          3
#define RING_BUFFER_ARRAY_TEXT_LENGHT   10
    



/****************************************************************************
 *                              Functoion's prototyps      
 ****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif


void MainFunction(void);

void HW_Init(void);
void SW_Init(void);
void InitEE(void);
void InitZeroCrossingInterrupt(void);
    
   

void timer_10ms(void);
void timer_50ms(void);
void timer_1Sec(void);
void timer_1Min(void);
void DisableSwitchs(void);
void EnableSwitchs(void);
void KeyStateMachine(void);

void ProvisionEvent(void);

void ToggleBreakerState(void);

void EventStateMachine(void);



void RefreshWatchDog(void);


void StartProvisioning(void);

void BreakerDCIInit( void );

void Ask_For_Temperature(void);
void DCI_UpdateSecondaryContactStatus(void);
void DCI_UpdatePrimaryContactStatus(void);

void DCI_UpdateSecondaryState(void);
void DCI_UpdatePrimaryState(void);
void DCI_UpdatePathStatus(void);
void DCI_UpdateIdentifyMeStatus(void);

void DCI_UpdatePowerUpState(uint8_t startup_config);

void DCI_Update_SSID(void);
void DCI_Update_DevicePassWord(void );
void DCI_Update_DeviceMacAdd(void);
void DCI_Update_FirmWarVer(void);
void DCI_UpdateSblcpEnDisStatus(void);

void GetDevicePassWord(void);
void GetDeviceSsid(void);

void DCI_UpdateBuildType(void);
void DCI_UpdateNewProgrammedPartNumber(void);

void DCI_Update_LoadPercent();
void DCI_Update_Temperature();
void DCI_UpdateDPSDeviceRegId(void);
void DCI_UpdateDPSEndpoint(void);
void DCI_UpdateDPSIDScope(void);
void DCI_UpdateDPSSymmetricKey(void);
void DCI_GetDPSDeviceRegId(void);
void DCI_GetDPSEndpoint(void);
void DCI_GetDPSIDScope(void);
void DCI_GetDPSSymmetricKey(void);

void initial_Uart(void);
uint8_t get_statusOfBuildType();




void Iot_Connect(void);
void Reset_provisioning(void);
void PxGreenOneSecondTimer(void);
void PxGreenConnectionStringChangedCB(void);
void PxGreenDpsConfigChangedCB(void);
void WiFi_Connect(void);
void Factory_WiFi_Connect(void);
void SetConnectStringInfo(void);
void GetConnectStringInfo(void);
void ResetConnectStringInfo(void);
void ParseConnectionString(void);

void SetDPSConcatenatedMessageToIndividualFields(void);
void GetDPSConcatenatedMessageFromIndividualFields(void);
void ParseDPSConcatenatedMessage(void);
void CreateDPSConcatenatedMessage(void);

void RF_ToggleBreakerState();     //delayed

void ResetDevice(uint8_t ResetType, uint8_t Reset);



void Test_AddingConnectionString(void);
void Test_AddingWifiSSID(void);
void Test_AddingWifiPWD(void);
void Test_RemoveWifiCredential(void);
void Test_ConnectWiFi(void);
void Test_WritingTo_NV_Config(void);
void Test_ReadingFrom_NV_Config(void);
void Test_WritingBuildType(void);
void Test_WritingNewPartNumber(void);
void Test_AddingDPSEndpoint(void);
void Test_AddingDPSIDScope(void);
void Test_AddingDPSDeviceRegId(void);
void Test_AddingDPSSymmetricKey(void);
void Test_ReadingDPSEndpoint(void);
void Test_ReadingDPSIDScope(void);
void Test_ReadingDPSDeviceRegId(void);
void Test_ReadingDPSSymmetricKey(void);
void Test_AddingDPSMessage(void);
void Test_ReadingDPSMessage(void);

void DCI_Update_VoltageL1(void);
void DCI_Update_VoltageL2(void);

void DCI_Update_CurrentL1(void);
void DCI_Update_CurrentL2(void);

void DCI_Update_PowerFactorL1(void);
void DCI_Update_PowerFactorL2(void);

void DCI_Update_FrequencyL1(void);
void DCI_Update_FrequencyL2(void);

void DCI_Update_ActivePowerL1(void);
void DCI_Update_ActivePowerL2(void);

void DCI_Update_ReactivePowerL1(void);
void DCI_Update_ReactivePowerL2(void);

void DCI_Update_ApparentPowerL1(void);
void DCI_Update_ApparentPowerL2(void);

void DCI_Update_ActiveEnergyL1(void);
void DCI_Update_ActiveEnergyL2(void);

void DCI_Update_ReactiveEnergyL1(void);
void DCI_Update_ReactiveEnergyL2(void);

void DCI_Update_ApparentEnergyL1(void);
void DCI_Update_ApparentEnergyL2(void);

void DCI_Update_RevActiveEnergyL1(void);
void DCI_Update_RevActiveEnergyL2(void);

void DCI_Update_RevReactiveEnergyL1(void);
void DCI_Update_RevReactiveEnergyL2(void);

void DCI_Update_RevApparentEnergyL1(void);
void DCI_Update_RevApparentEnergyL2(void);

void DCI_Update_ErroLogUpdate(const char * ErrorLog);

void ErrorLogOneSecondTimer(void);

uint32_t GetEpochTime( void );
uint8_t get_factoryModeDisableStatus();
int8_t get_sblcpManufacturingModeOnStatus();
void sblcp_updateReplyMsgStatus(uint8_t);
bool IsDeviceInProvisioningMode(void);
int lockSB2Device();

eTaskState Get_Task_State(TaskHandle_t task_handle);

void DCI_Update_Wifi_RSS_Signal_Strength(void);

void UART_Send_Event_To_Queue(void *puart_m2m_data);

#ifdef __cplusplus
}
#endif


#endif //__Breaker__H

