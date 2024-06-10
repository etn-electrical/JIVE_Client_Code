/*
*****************************************************************************************
*       $Workfile:
*
*       $Author:$
*       $Date:$
*       Copyright© 2011, Eaton Corporation. All Rights Reserved.
*
*       $Header:$
*****************************************************************************************
*/
#include "Includes.h"
#include "REST_DCI_Data.h"
#include "Format_Handler.h"
#include "Prod_Spec_Format_Handler.h"


//*******************************************************
//******     The RESTful misc definitions 
//*******************************************************

static const rest_pid16_t REST_DCI_DATA_ALL_PARAM_ACCESS_R= 7U;
static const rest_pid16_t REST_DCI_DATA_ALL_PARAM_ACCESS_W= 99U;
static const rest_pid16_t NUM_TOTAL_REST_DCI_DATA_PARAMS= 119U;


static const rest_pid16_t DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ID= 0U;
static const rest_pid16_t DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_ID= 1U;
static const rest_pid16_t DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_ID= 2U;
static const rest_pid16_t DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_ID= 3U;
static const rest_pid16_t DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_ID= 4U;
static const rest_pid16_t DCI_PHY1_AUTONEG_STATE_REST_PARAM_ID= 5U;
static const rest_pid16_t DCI_PHY1_PORT_ENABLE_REST_PARAM_ID= 6U;
static const rest_pid16_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ID= 14U;
static const rest_pid16_t DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_ID= 15U;
static const rest_pid16_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ID= 16U;
static const rest_pid16_t DCI_ETH_ACD_ENABLE_REST_PARAM_ID= 24U;
static const rest_pid16_t DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_ID= 25U;
static const rest_pid16_t DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_ID= 26U;
static const rest_pid16_t DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_ID= 27U;
static const rest_pid16_t DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_ID= 28U;
static const rest_pid16_t DCI_PRODUCT_SERIAL_NUM_REST_PARAM_ID= 29U;
static const rest_pid16_t DCI_VENDOR_NAME_REST_PARAM_ID= 30U;
static const rest_pid16_t DCI_PRODUCT_CODE_REST_PARAM_ID= 31U;
static const rest_pid16_t DCI_PRODUCT_CODE_ASCII_REST_PARAM_ID= 32U;
static const rest_pid16_t DCI_VENDOR_URL_REST_PARAM_ID= 33U;
static const rest_pid16_t DCI_PRODUCT_NAME_REST_PARAM_ID= 34U;
static const rest_pid16_t DCI_MODEL_NAME_REST_PARAM_ID= 35U;
static const rest_pid16_t DCI_USER_APP_NAME_REST_PARAM_ID= 36U;
static const rest_pid16_t DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_ID= 37U;
static const rest_pid16_t DCI_MULTICAST_ENABLE_REST_PARAM_ID= 38U;
static const rest_pid16_t DCI_BROADCAST_ENABLE_REST_PARAM_ID= 39U;
static const rest_pid16_t DCI_APP_FIRM_VER_NUM_REST_PARAM_ID= 2010U;
static const rest_pid16_t DCI_WEB_FIRM_VER_NUM_REST_PARAM_ID= 2011U;
static const rest_pid16_t DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_ID= 2028U;
static const rest_pid16_t DCI_ACTIVE_IP_ADDRESS_REST_PARAM_ID= 2030U;
static const rest_pid16_t DCI_ACTIVE_SUBNET_MASK_REST_PARAM_ID= 2031U;
static const rest_pid16_t DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_ID= 2032U;
static const rest_pid16_t DCI_STATIC_IP_ADDRESS_REST_PARAM_ID= 2033U;
static const rest_pid16_t DCI_STATIC_SUBNET_MASK_REST_PARAM_ID= 2034U;
static const rest_pid16_t DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_ID= 2035U;
static const rest_pid16_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ID= 10000U;
static const rest_pid16_t DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_ID= 10014U;
static const rest_pid16_t DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_ID= 10015U;
static const rest_pid16_t DCI_LTK_VER_ASCII_REST_PARAM_ID= 10016U;
static const rest_pid16_t DCI_FW_UPGRADE_MODE_REST_PARAM_ID= 10017U;
static const rest_pid16_t DCI_USER_WEB_FW_NAME_REST_PARAM_ID= 10024U;
static const rest_pid16_t DCI_PRODUCT_PROC_NAME_REST_PARAM_ID= 10025U;
static const rest_pid16_t DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_ID= 10026U;
static const rest_pid16_t DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_ID= 10027U;
static const rest_pid16_t DCI_HTTP_ENABLE_REST_PARAM_ID= 10028U;
static const rest_pid16_t DCI_TRUSTED_IP_WHITELIST_REST_PARAM_ID= 10029U;
static const rest_pid16_t DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_ID= 10030U;
static const rest_pid16_t DCI_UNIX_EPOCH_TIME_REST_PARAM_ID= 10032U;
static const rest_pid16_t DCI_TIME_OFFSET_REST_PARAM_ID= 10033U;
static const rest_pid16_t DCI_DATE_FORMAT_REST_PARAM_ID= 10034U;
static const rest_pid16_t DCI_TIME_FORMAT_REST_PARAM_ID= 10035U;
static const rest_pid16_t DCI_REST_RESET_COMMAND_REST_PARAM_ID= 10036U;
static const rest_pid16_t DCI_DEVICE_CERT_VALID_REST_PARAM_ID= 10037U;
static const rest_pid16_t DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ID= 10038U;
static const rest_pid16_t DCI_SNTP_SERVER_1_REST_PARAM_ID= 10039U;
static const rest_pid16_t DCI_SNTP_SERVER_2_REST_PARAM_ID= 10040U;
static const rest_pid16_t DCI_SNTP_SERVER_3_REST_PARAM_ID= 10041U;
static const rest_pid16_t DCI_SNTP_UPDATE_TIME_REST_PARAM_ID= 10042U;
static const rest_pid16_t DCI_SNTP_RETRY_TIME_REST_PARAM_ID= 10043U;
static const rest_pid16_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ID= 10044U;
static const rest_pid16_t DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_ID= 10045U;
static const rest_pid16_t DCI_IOT_CONNECT_REST_PARAM_ID= 10046U;
static const rest_pid16_t DCI_IOT_STATUS_REST_PARAM_ID= 10047U;
static const rest_pid16_t DCI_IOT_PROXY_ENABLE_REST_PARAM_ID= 10048U;
static const rest_pid16_t DCI_IOT_PROXY_SERVER_REST_PARAM_ID= 10049U;
static const rest_pid16_t DCI_IOT_PROXY_PORT_REST_PARAM_ID= 10050U;
static const rest_pid16_t DCI_IOT_PROXY_USERNAME_REST_PARAM_ID= 10051U;
static const rest_pid16_t DCI_IOT_PROXY_PASSWORD_REST_PARAM_ID= 10052U;
static const rest_pid16_t DCI_IOT_DEVICE_GUID_REST_PARAM_ID= 10053U;
static const rest_pid16_t DCI_IOT_DEVICE_PROFILE_REST_PARAM_ID= 10054U;
static const rest_pid16_t DCI_IOT_CONN_STRING_REST_PARAM_ID= 10055U;
static const rest_pid16_t DCI_IOT_UPDATE_RATE_REST_PARAM_ID= 10056U;
static const rest_pid16_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ID= 10057U;
static const rest_pid16_t DCI_GROUP0_CADENCE_REST_PARAM_ID= 10058U;
static const rest_pid16_t DCI_GROUP1_CADENCE_REST_PARAM_ID= 10059U;
static const rest_pid16_t DCI_GROUP2_CADENCE_REST_PARAM_ID= 10060U;
static const rest_pid16_t DCI_GROUP3_CADENCE_REST_PARAM_ID= 10061U;
static const rest_pid16_t DCI_LOG_TEST_1_REST_PARAM_ID= 10062U;
static const rest_pid16_t DCI_RESET_CAUSE_REST_PARAM_ID= 10063U;
static const rest_pid16_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ID= 10064U;
static const rest_pid16_t DCI_DEVICE_STATE_REST_PARAM_ID= 10065U;
static const rest_pid16_t DCI_LOG_EVENT_TEST_101_REST_PARAM_ID= 10066U;
static const rest_pid16_t DCI_LOG_EVENT_TEST_102_REST_PARAM_ID= 10067U;
static const rest_pid16_t DCI_NV_MEM_ID_REST_PARAM_ID= 10068U;
static const rest_pid16_t DCI_NV_PARAMETER_ADDRESS_REST_PARAM_ID= 10069U;
static const rest_pid16_t DCI_NV_FAIL_OPERATION_REST_PARAM_ID= 10070U;
static const rest_pid16_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ID= 10071U;
static const rest_pid16_t DCI_LOG_FIRMWARE_VERSION_REST_PARAM_ID= 10072U;
static const rest_pid16_t DCI_LOGGED_USERNAME_REST_PARAM_ID= 10073U;
static const rest_pid16_t DCI_LOG_IP_ADDRESS_REST_PARAM_ID= 10074U;
static const rest_pid16_t DCI_LOG_PORT_NUMBER_REST_PARAM_ID= 10075U;
static const rest_pid16_t DCI_MODBUS_TCP_ENABLE_REST_PARAM_ID= 10076U;
static const rest_pid16_t DCI_BACNET_IP_ENABLE_REST_PARAM_ID= 10077U;
static const rest_pid16_t DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_ID= 10078U;
static const rest_pid16_t DCI_MAX_CONCURRENT_SESSION_REST_PARAM_ID= 10079U;
static const rest_pid16_t DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_ID= 10080U;
static const rest_pid16_t DCI_USER_LOCK_TIME_SEC_REST_PARAM_ID= 10081U;
static const rest_pid16_t DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_ID= 10082U;
static const rest_pid16_t DCI_CORS_TYPE_REST_PARAM_ID= 10083U;
static const rest_pid16_t DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_ID= 10084U;
static const rest_pid16_t DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_ID= 10085U;
static const rest_pid16_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_ID= 10086U;
static const rest_pid16_t DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_ID= 10087U;
static const rest_pid16_t DCI_MDNS_SERVER1_NAME_REST_PARAM_ID= 10088U;
static const rest_pid16_t DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_ID= 10089U;
static const rest_pid16_t DCI_MDNS_SERVER1_PORT_REST_PARAM_ID= 10090U;
static const rest_pid16_t DCI_LICENSE_INFORMATION_REST_PARAM_ID= 10091U;
static const rest_pid16_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ID= 10130U;
static const rest_pid16_t DCI_LANG_PREF_SETTING_REST_PARAM_ID= 10131U;
static const rest_pid16_t DCI_COMMON_LANG_PREF_REST_PARAM_ID= 10132U;
static const rest_pid16_t DCI_DATA_LOG_REST_PARAM_ID= 10133U;
static const rest_pid16_t DCI_EVENT_LOG_REST_PARAM_ID= 10134U;
static const rest_pid16_t DCI_AUDIT_POWER_LOG_REST_PARAM_ID= 10135U;
static const rest_pid16_t DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_ID= 10136U;
static const rest_pid16_t DCI_AUDIT_USER_LOG_REST_PARAM_ID= 10137U;
static const rest_pid16_t DCI_AUDIT_CONFIG_LOG_REST_PARAM_ID= 10138U;
static const rest_pid16_t DCI_DEVICE_UP_TIME_SEC_REST_PARAM_ID= 10139U;
static const rest_pid16_t DCI_PTP_ENABLE_REST_PARAM_ID= 10140U;
static const rest_pid16_t DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_ID= 10141U;

//*******************************************************
//******     The RESTful parameter names 
//*******************************************************

#ifndef REMOVE_REST_PARAM_NAME_TEXT
static const char_t DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_NAME []= "Eth Port 1 Speed Select";
static const char_t DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_NAME []= "Eth Port 1 Speed Actual";
static const char_t DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_NAME []= "Eth Port 1 Full Duplex Enable";
static const char_t DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_NAME []= "Eth Port 1 Full Duplex Status";
static const char_t DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_NAME []= "Eth Port 1 Autonegotiate Enable";
static const char_t DCI_PHY1_AUTONEG_STATE_REST_PARAM_NAME []= "Eth Port 1 Autonegotiate State";
static const char_t DCI_PHY1_PORT_ENABLE_REST_PARAM_NAME []= "Eth Port 1 Enabled";
static const char_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_NAME []= "Status of Control of IP Address Allocation Method is from DIP switch or Ethernet";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_NAME []= "Current Method of IP Allocation";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_NAME []= "Method of IP Allocation";
static const char_t DCI_ETH_ACD_ENABLE_REST_PARAM_NAME []= "ACD Enable";
static const char_t DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_NAME []= "ACD Status";
static const char_t DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_NAME []= "ACD Conflicted Status";
static const char_t DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_NAME []= "ACD Conflicted MAC";
static const char_t DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_NAME []= "ACD Conflicted PDU";
static const char_t DCI_PRODUCT_SERIAL_NUM_REST_PARAM_NAME []= "Serial Number";
static const char_t DCI_VENDOR_NAME_REST_PARAM_NAME []= "Vendor Name";
static const char_t DCI_PRODUCT_CODE_REST_PARAM_NAME []= "Product Code";
static const char_t DCI_PRODUCT_CODE_ASCII_REST_PARAM_NAME []= "ASCII Product Code";
static const char_t DCI_VENDOR_URL_REST_PARAM_NAME []= "Vendor URL";
static const char_t DCI_PRODUCT_NAME_REST_PARAM_NAME []= "Product Name";
static const char_t DCI_MODEL_NAME_REST_PARAM_NAME []= "Model Name";
static const char_t DCI_USER_APP_NAME_REST_PARAM_NAME []= "Assigned Name";
static const char_t DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_NAME []= "Modbus TCP Com Timeout";
static const char_t DCI_MULTICAST_ENABLE_REST_PARAM_NAME []= "Multicast Enable Disable";
static const char_t DCI_BROADCAST_ENABLE_REST_PARAM_NAME []= "Broadcast Enable Disable";
static const char_t DCI_APP_FIRM_VER_NUM_REST_PARAM_NAME []= "App Firmware Ver Num";
static const char_t DCI_WEB_FIRM_VER_NUM_REST_PARAM_NAME []= "Web Firmware Ver Num";
static const char_t DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_NAME []= "Ethernet MAC Address";
static const char_t DCI_ACTIVE_IP_ADDRESS_REST_PARAM_NAME []= "Present Ethernet IP Address";
static const char_t DCI_ACTIVE_SUBNET_MASK_REST_PARAM_NAME []= "Present Ethernet Subnet Mask";
static const char_t DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_NAME []= "Present Ethernet Default Gateway";
static const char_t DCI_STATIC_IP_ADDRESS_REST_PARAM_NAME []= "Stored Ethernet IP Address";
static const char_t DCI_STATIC_SUBNET_MASK_REST_PARAM_NAME []= "Stored Ethernet Subnet Mask";
static const char_t DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_NAME []= "Stored Ethernet Default Gateway";
static const char_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_NAME []= "Password Waiver Level";
static const char_t DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_NAME []= "Simple App Control word";
static const char_t DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_NAME []= "Simple App status word";
static const char_t DCI_LTK_VER_ASCII_REST_PARAM_NAME []= "LTK Rev ASCII";
static const char_t DCI_FW_UPGRADE_MODE_REST_PARAM_NAME []= "Firmware Upgrade Mode";
static const char_t DCI_USER_WEB_FW_NAME_REST_PARAM_NAME []= "Assigned Name";
static const char_t DCI_PRODUCT_PROC_NAME_REST_PARAM_NAME []= "Product Name";
static const char_t DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_NAME []= "Serial Number";
static const char_t DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_NAME []= "User account Inactivity timeout";
static const char_t DCI_HTTP_ENABLE_REST_PARAM_NAME []= "HTTP";
static const char_t DCI_TRUSTED_IP_WHITELIST_REST_PARAM_NAME []= "Trusted IP Address filter white list";
static const char_t DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_NAME []= "Trusted IP filter list Modbus/TCP Enable";
static const char_t DCI_UNIX_EPOCH_TIME_REST_PARAM_NAME []= "Set Time";
static const char_t DCI_TIME_OFFSET_REST_PARAM_NAME []= "Timezone";
static const char_t DCI_DATE_FORMAT_REST_PARAM_NAME []= "Date Format";
static const char_t DCI_TIME_FORMAT_REST_PARAM_NAME []= "Time Format";
static const char_t DCI_REST_RESET_COMMAND_REST_PARAM_NAME []= "Reboot or Reset Device";
static const char_t DCI_DEVICE_CERT_VALID_REST_PARAM_NAME []= "Device Certificate Validity Status";
static const char_t DCI_SNTP_SERVICE_ENABLE_REST_PARAM_NAME []= "SNTP Operation Enable";
static const char_t DCI_SNTP_SERVER_1_REST_PARAM_NAME []= "SNTP server 1";
static const char_t DCI_SNTP_SERVER_2_REST_PARAM_NAME []= "SNTP server 2";
static const char_t DCI_SNTP_SERVER_3_REST_PARAM_NAME []= "SNTP server 3";
static const char_t DCI_SNTP_UPDATE_TIME_REST_PARAM_NAME []= "SNTP Update Time";
static const char_t DCI_SNTP_RETRY_TIME_REST_PARAM_NAME []= "SNTP Retry Time";
static const char_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_NAME []= "SNTP Active Server Status";
static const char_t DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_NAME []= "Set Time 64bit;Seconds;Microseconds";
static const char_t DCI_IOT_CONNECT_REST_PARAM_NAME []= "IOT Enable or Disable";
static const char_t DCI_IOT_STATUS_REST_PARAM_NAME []= "IOT Connection Status";
static const char_t DCI_IOT_PROXY_ENABLE_REST_PARAM_NAME []= "Proxy Enable";
static const char_t DCI_IOT_PROXY_SERVER_REST_PARAM_NAME []= "Proxy Server Address";
static const char_t DCI_IOT_PROXY_PORT_REST_PARAM_NAME []= "Proxy Server Port";
static const char_t DCI_IOT_PROXY_USERNAME_REST_PARAM_NAME []= "Proxy Username";
static const char_t DCI_IOT_PROXY_PASSWORD_REST_PARAM_NAME []= "Proxy Password";
static const char_t DCI_IOT_DEVICE_GUID_REST_PARAM_NAME []= "Device GUID";
static const char_t DCI_IOT_DEVICE_PROFILE_REST_PARAM_NAME []= "Device Profile";
static const char_t DCI_IOT_CONN_STRING_REST_PARAM_NAME []= "IOT Hub Server Connection string";
static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_NAME []= "IOT Data Update Rate";
static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_NAME []= "IOT Cadence Update Rate";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_NAME []= "Group 0 Cadence";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_NAME []= "Group 1 Cadence";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_NAME []= "Group 2 Cadence";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_NAME []= "Group 3 Cadence";
static const char_t DCI_LOG_TEST_1_REST_PARAM_NAME []= "Log test";
static const char_t DCI_RESET_CAUSE_REST_PARAM_NAME []= "Reset Cause";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_NAME []= "Logging Event";
static const char_t DCI_DEVICE_STATE_REST_PARAM_NAME []= "Device State";
static const char_t DCI_LOG_EVENT_TEST_101_REST_PARAM_NAME []= "Event Log test 101";
static const char_t DCI_LOG_EVENT_TEST_102_REST_PARAM_NAME []= "Event Log test 102";
static const char_t DCI_NV_MEM_ID_REST_PARAM_NAME []= "NV Memory ID";
static const char_t DCI_NV_PARAMETER_ADDRESS_REST_PARAM_NAME []= "NV Parameter Address";
static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_NAME []= "NV Operation";
static const char_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_NAME []= "Processor and Image";
static const char_t DCI_LOG_FIRMWARE_VERSION_REST_PARAM_NAME []= "Firmware Version";
static const char_t DCI_LOGGED_USERNAME_REST_PARAM_NAME []= "Logged user name";
static const char_t DCI_LOG_IP_ADDRESS_REST_PARAM_NAME []= "IP Address";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_NAME []= "Port";
static const char_t DCI_MODBUS_TCP_ENABLE_REST_PARAM_NAME []= "Modbus TCP";
static const char_t DCI_BACNET_IP_ENABLE_REST_PARAM_NAME []= "Bacnet IP";
static const char_t DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_NAME []= "Enable Session Takeover";
static const char_t DCI_MAX_CONCURRENT_SESSION_REST_PARAM_NAME []= "Maximum Concurrent Sessions";
static const char_t DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_NAME []= "User Absolute Timeout";
static const char_t DCI_USER_LOCK_TIME_SEC_REST_PARAM_NAME []= "User Lock Time";
static const char_t DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_NAME []= "Maximum Failed Login Attempts";
static const char_t DCI_CORS_TYPE_REST_PARAM_NAME []= "CORS Origin Type";
static const char_t DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_NAME []= "Trusted IP filter list BACnet IP Enable";
static const char_t DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_NAME []= "Trusted IP filter list EIP Enable";
static const char_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_NAME []= "Device Certificate Generation Control";
static const char_t DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_NAME []= "LWM2M Service Discovery Enable";
static const char_t DCI_MDNS_SERVER1_NAME_REST_PARAM_NAME []= "MDNS Server 1 Local Domain Name";
static const char_t DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_NAME []= "MDNS Server 1 IP Address";
static const char_t DCI_MDNS_SERVER1_PORT_REST_PARAM_NAME []= "MDNS Server 1 Service Port Number";
static const char_t DCI_LICENSE_INFORMATION_REST_PARAM_NAME []= "Open Source License";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_NAME []= "IOT Connection status reason";
static const char_t DCI_LANG_PREF_SETTING_REST_PARAM_NAME []= "Preferred Language Setting";
static const char_t DCI_COMMON_LANG_PREF_REST_PARAM_NAME []= "Common Language Preferrence";
static const char_t DCI_DATA_LOG_REST_PARAM_NAME []= "Data Log";
static const char_t DCI_EVENT_LOG_REST_PARAM_NAME []= "Event Log";
static const char_t DCI_AUDIT_POWER_LOG_REST_PARAM_NAME []= "Audit-Power Log";
static const char_t DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_NAME []= "Audit-Fw-Update Log";
static const char_t DCI_AUDIT_USER_LOG_REST_PARAM_NAME []= "Audit-User Log";
static const char_t DCI_AUDIT_CONFIG_LOG_REST_PARAM_NAME []= "Audit-Config Log";
static const char_t DCI_DEVICE_UP_TIME_SEC_REST_PARAM_NAME []= "Device Up Time";
static const char_t DCI_PTP_ENABLE_REST_PARAM_NAME []= "Enable or Disable PTP";
static const char_t DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_NAME []= "SNTP Time offset debug";
#endif

//*******************************************************
//******     The RESTful descriptions 
//*******************************************************

#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
static const char_t DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_DESCRIPTION []= "Select the Ethernet link speed.  Only used when Auto-Negotiate is disabled.  \nValid values: 10, 100";
static const char_t DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_DESCRIPTION []= "Actual Ethernet link speed.";
static const char_t DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_DESCRIPTION []= "Selects the duplex mode.  Only used when Auto-Negotiate is disabled.\nTrue = Full Duplex\nFalse = Half Duplex";
static const char_t DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_DESCRIPTION []= "Actual duplex mode.\nTrue = Full Duplex\nFalse = Half Duplex";
static const char_t DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_DESCRIPTION []= "Selects Auto-Negotiation of link speed and duplex.  \nTrue = Enabled";
static const char_t DCI_PHY1_AUTONEG_STATE_REST_PARAM_DESCRIPTION []= "Active state of the Auto-Negotiation behavior.\nValue = State\n0 = Link Inactive\n1 = Auto Negotiation in Progress\n2 = Auto Negotiation Failed (Defafult used)\n3 = Auto Negotiation of Duplex Failed (speed ok)\n4 = Auto Negotiation Success\n5 = Auto Negotiation Disabled\n6 = Port Disabled";
static const char_t DCI_PHY1_PORT_ENABLE_REST_PARAM_DESCRIPTION []= "This parameter can disable the Ethernet Port.  Effectively disabling the port.\nTrue = Enable Port\nFalse = Disable Port";
static const char_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_DESCRIPTION []= "Status of Control of IP Address Allocation Method is from DIP switch or Ethernet.\n1= DIP Switch\n2=Ethernet";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_DESCRIPTION []= "Shows the present method used to allocate an IP Address:\n0 - Restore (hardcoded 192.168.1.254).\n1 - DHCP.\n2 - Full address taken from NV Memory.\n3 - Upper three octets from NV and lower octet selected by the dip switch setting.";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_DESCRIPTION []= "Allows to set the method used to allocate an IP Address:\n0 - Restore (hardcoded 192.168.1.254).\n1 - DHCP.\n2 - Full address taken from NV Memory.\n3 - Upper three octets from NV and lower octet selected by the dip switch setting.\n(Critical parameters - Care must be taken while switching to different method)";
static const char_t DCI_ETH_ACD_ENABLE_REST_PARAM_DESCRIPTION []= "Address Conflict Detection enable.  ACD provides protection from duplicate IP addresses on the network.";
static const char_t DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_DESCRIPTION []= "Address Conflict Detection State.  \nVal = Description\n0 = No Conflict Detected\n1 = Conflict Detected Defending\n2 = Conflict Detected Retreated";
static const char_t DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_DESCRIPTION []= "Address Conflict Detection Status.  The state of ACD activity when last conflict was detected.\nVal = Description\n0 = No Conflict\n1 = Probe IPV4 Address\n2 = Ongoing Detection\n3 = Semi-Active Probing";
static const char_t DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_DESCRIPTION []= "Address Conflict Detection conflicted device MAC address.  The source MAC address from the header of the received Ethernet packet which was sent by a device reporting a conflict.";
static const char_t DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_DESCRIPTION []= "Address Conflict Detection raw ARP PDU when conflict was detected.";
static const char_t DCI_PRODUCT_SERIAL_NUM_REST_PARAM_DESCRIPTION []= "32bit Device Serial Number. (MAE)";
static const char_t DCI_VENDOR_NAME_REST_PARAM_DESCRIPTION []= "Product vendor name character string";
static const char_t DCI_PRODUCT_CODE_REST_PARAM_DESCRIPTION []= "Product code numerical";
static const char_t DCI_PRODUCT_CODE_ASCII_REST_PARAM_DESCRIPTION []= "Product code character string";
static const char_t DCI_VENDOR_URL_REST_PARAM_DESCRIPTION []= "Product vendor URL";
static const char_t DCI_PRODUCT_NAME_REST_PARAM_DESCRIPTION []= "Product Name";
static const char_t DCI_MODEL_NAME_REST_PARAM_DESCRIPTION []= "Product model name";
static const char_t DCI_USER_APP_NAME_REST_PARAM_DESCRIPTION []= "User assigned name";
static const char_t DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_DESCRIPTION []= "Communication timeout for Modbus TCP.  0 = Disable.";
static const char_t DCI_MULTICAST_ENABLE_REST_PARAM_DESCRIPTION []= "Multicast settings:\nValue   : Description\n0x00000010: Multicast Enable\n0x00000000: Multicast Disable with Filter PerfectFilter\n0x00000004: Multicast Disable with Filter HashTable\n0x00000404: Multicast Disable with Filter_PerfectHashTable";
static const char_t DCI_BROADCAST_ENABLE_REST_PARAM_DESCRIPTION []= "Broadcast Enable Disable settings:\n0x00000000: BroadcastFramesReception_Enable\n0x00000020: BroadcastFramesReception_Disable";
static const char_t DCI_APP_FIRM_VER_NUM_REST_PARAM_DESCRIPTION []= "App Firmware Version Number";
static const char_t DCI_WEB_FIRM_VER_NUM_REST_PARAM_DESCRIPTION []= "Web Firmware Version Number";
static const char_t DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_DESCRIPTION []= "Unique MAC Address assigned to this device.(MAE)";
static const char_t DCI_ACTIVE_IP_ADDRESS_REST_PARAM_DESCRIPTION []= "The active IP address being used on the network.";
static const char_t DCI_ACTIVE_SUBNET_MASK_REST_PARAM_DESCRIPTION []= "The active subnet mask IP address being used on the network.";
static const char_t DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_DESCRIPTION []= "The active default gateway IP address being used on the network.";
static const char_t DCI_STATIC_IP_ADDRESS_REST_PARAM_DESCRIPTION []= "The IP address used in the NV address select configuration.";
static const char_t DCI_STATIC_SUBNET_MASK_REST_PARAM_DESCRIPTION []= "The IP subnet mask used in the NV address select configuration.";
static const char_t DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_DESCRIPTION []= "The IP default gateway used in the NV address select configuration.";
static const char_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_DESCRIPTION []= "Any level lower than or equal to this value does not need a password";
static const char_t DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_DESCRIPTION []= "Simple application control word";
static const char_t DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_DESCRIPTION []= "Simple application status word";
static const char_t DCI_LTK_VER_ASCII_REST_PARAM_DESCRIPTION []= "LTK revision ASCII";
static const char_t DCI_FW_UPGRADE_MODE_REST_PARAM_DESCRIPTION []= "Firmware Upgrade Mode";
static const char_t DCI_USER_WEB_FW_NAME_REST_PARAM_DESCRIPTION []= "User assigned name";
static const char_t DCI_PRODUCT_PROC_NAME_REST_PARAM_DESCRIPTION []= "Product Name";
static const char_t DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_DESCRIPTION []= "32bit Device Serial Number. (MAE)";
static const char_t DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_DESCRIPTION []= "User account Inactivity timeout In seconds";
static const char_t DCI_HTTP_ENABLE_REST_PARAM_DESCRIPTION []= "HTTP is an insecure protocol and enabling it can lead to security risk";
static const char_t DCI_TRUSTED_IP_WHITELIST_REST_PARAM_DESCRIPTION []= "Enter 3 comma separated IPs or range.\n Eg. 192.168.1.25 (Fixed IP), 166.99.170.255 (Range 166.99.170.x) , 172.48.255.255 (Range : 172.48.x.x). \nWhere &apos;X&apos; value range from 1-255.";
static const char_t DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_DESCRIPTION []= "Trusted IP filter list Modbus/TCP Enable";
static const char_t DCI_UNIX_EPOCH_TIME_REST_PARAM_DESCRIPTION []= "32 bit epoch Time from RTC";
static const char_t DCI_TIME_OFFSET_REST_PARAM_DESCRIPTION []= "Timezone value will be set with offset. E.g GMT+530 value would be -330";
static const char_t DCI_DATE_FORMAT_REST_PARAM_DESCRIPTION []= "Date format values\n0 = mm/dd/yyyy\n1 = dd/mm/yyyy\n2 = yyyy-mm-dd\n3 = dd mm yyyy";
static const char_t DCI_TIME_FORMAT_REST_PARAM_DESCRIPTION []= "Time format can be\n0 - 12Hrs\n1- 24Hrs";
static const char_t DCI_REST_RESET_COMMAND_REST_PARAM_DESCRIPTION []= "Reboot Device is soft reset, device will be power down and power up. Factory Reset is hard reset, all non-volatile data will be wiped out. Please refresh browser once you save it.";
static const char_t DCI_DEVICE_CERT_VALID_REST_PARAM_DESCRIPTION []= "Device Certificate Validity Status\n0 - Certificate Invalid\n1 - Certificate Valid";
static const char_t DCI_SNTP_SERVICE_ENABLE_REST_PARAM_DESCRIPTION []= "SNTP Operation Enable";
static const char_t DCI_SNTP_SERVER_1_REST_PARAM_DESCRIPTION []= "The SNTP Server 1 IP address or Domain Name";
static const char_t DCI_SNTP_SERVER_2_REST_PARAM_DESCRIPTION []= "The SNTP Server 2 IP address or Domain Name";
static const char_t DCI_SNTP_SERVER_3_REST_PARAM_DESCRIPTION []= "The SNTP Server 3 IP address or Domain Name";
static const char_t DCI_SNTP_UPDATE_TIME_REST_PARAM_DESCRIPTION []= "SNTP Update Time";
static const char_t DCI_SNTP_RETRY_TIME_REST_PARAM_DESCRIPTION []= "SNTP Retry time will be doubled when no server is responding. It will increased upto 10 times of retry time which user has configured.";
static const char_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_DESCRIPTION []= "SNTP Active Server Status";
static const char_t DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_DESCRIPTION []= "64 bit (32 bit in seconds and 32 bit in microseconds) epoch Time from RTC";
static const char_t DCI_IOT_CONNECT_REST_PARAM_DESCRIPTION []= "Control button to connect or disconnect to IOT(connect = 1, disconnect = 0)";
static const char_t DCI_IOT_STATUS_REST_PARAM_DESCRIPTION []= "Show the current status with the IOT like connected/disconnected(connected = 1, disconnected = 0)";
static const char_t DCI_IOT_PROXY_ENABLE_REST_PARAM_DESCRIPTION []= "This will be used to use or bypass Proxy server settings (Proxy Enable = 1, Proxy Disable = 0)";
static const char_t DCI_IOT_PROXY_SERVER_REST_PARAM_DESCRIPTION []= "Proxy server address";
static const char_t DCI_IOT_PROXY_PORT_REST_PARAM_DESCRIPTION []= "Proxy server port";
static const char_t DCI_IOT_PROXY_USERNAME_REST_PARAM_DESCRIPTION []= "User name used to login in the IOT hub server";
static const char_t DCI_IOT_PROXY_PASSWORD_REST_PARAM_DESCRIPTION []= "Password used to login in the IOT hub server";
static const char_t DCI_IOT_DEVICE_GUID_REST_PARAM_DESCRIPTION []= "Device GUID received from PX white";
static const char_t DCI_IOT_DEVICE_PROFILE_REST_PARAM_DESCRIPTION []= "Device Profile received from PX white";
static const char_t DCI_IOT_CONN_STRING_REST_PARAM_DESCRIPTION []= "Connection string to access IOT Hub server";
static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_DESCRIPTION []= "Data update rate from device to cloud";
static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_DESCRIPTION []= "Cadence data update rate from device to cloud";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_DESCRIPTION []= "10s cadence, SeeMe type";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_DESCRIPTION []= "60s cadence, StoreMe on interval type";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_DESCRIPTION []= "1s cadence, SeeMe type";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_DESCRIPTION []= "1s cadence, StoreMe type";
static const char_t DCI_LOG_TEST_1_REST_PARAM_DESCRIPTION []= "Test variable for logging test";
static const char_t DCI_RESET_CAUSE_REST_PARAM_DESCRIPTION []= "Provides the cause of a reset.  \nBit = Description\n0 = Power Up\n1 = Wakeup\n2 = Watchdog\n3 = Software\n4 = Reset Pin\n5 = Brown Out\n6 = Undefined";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_DESCRIPTION []= "";
static const char_t DCI_DEVICE_STATE_REST_PARAM_DESCRIPTION []= "";
static const char_t DCI_LOG_EVENT_TEST_101_REST_PARAM_DESCRIPTION []= "";
static const char_t DCI_LOG_EVENT_TEST_102_REST_PARAM_DESCRIPTION []= "";
static const char_t DCI_NV_MEM_ID_REST_PARAM_DESCRIPTION []= "";
static const char_t DCI_NV_PARAMETER_ADDRESS_REST_PARAM_DESCRIPTION []= "";
static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_DESCRIPTION []= "";
static const char_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_DESCRIPTION []= "Processor and Image ID";
static const char_t DCI_LOG_FIRMWARE_VERSION_REST_PARAM_DESCRIPTION []= "Firmware Version parameter used for logging";
static const char_t DCI_LOGGED_USERNAME_REST_PARAM_DESCRIPTION []= "Logged user name";
static const char_t DCI_LOG_IP_ADDRESS_REST_PARAM_DESCRIPTION []= "The IP address of client";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_DESCRIPTION []= "Port number";
static const char_t DCI_MODBUS_TCP_ENABLE_REST_PARAM_DESCRIPTION []= "Modbus TCP is an insecure protocol and enabling it can lead to security risk";
static const char_t DCI_BACNET_IP_ENABLE_REST_PARAM_DESCRIPTION []= "Bacnet IP is an insecure protocol and enabling it can lead to security risk";
static const char_t DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_DESCRIPTION []= "Enable/Disable the session takeover(enable = 1, disable = 0)";
static const char_t DCI_MAX_CONCURRENT_SESSION_REST_PARAM_DESCRIPTION []= "Maximum number of sessions a user can have.";
static const char_t DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_DESCRIPTION []= "Maximum time(sec) after which a session will be terminated automatically";
static const char_t DCI_USER_LOCK_TIME_SEC_REST_PARAM_DESCRIPTION []= "A user will be locked for this period of time(sec) if maximum failed login attempt is achieved";
static const char_t DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_DESCRIPTION []= "Maimum login attempts a user can have before before account lock";
static const char_t DCI_CORS_TYPE_REST_PARAM_DESCRIPTION []= "0 - request from all origin allowed\n1 - allow request having origin as active IP ";
static const char_t DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_DESCRIPTION []= "Trusted IP filter list  BACnet IP Enable";
static const char_t DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_DESCRIPTION []= "Trusted IP filter list  EIP Enable";
static const char_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_DESCRIPTION []= "Device Certificate Generation Control\n0 - Certificate Generation Disable. If disabled certificate will NOT generate after factory reset.\n1 - Certificate Generation Enable. If enabled certificate will generate after factory reset.\n2 - Certificate Generation Enable on IP change. If enabled certificate will generate on boot-up if IP is changed.";
static const char_t DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_DESCRIPTION []= "LWM2M Service Discovery Enable";
static const char_t DCI_MDNS_SERVER1_NAME_REST_PARAM_DESCRIPTION []= "MDNS Server 1 Local Domain Name";
static const char_t DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_DESCRIPTION []= "MDNS Server 1 IP Address";
static const char_t DCI_MDNS_SERVER1_PORT_REST_PARAM_DESCRIPTION []= "MDNS Server 1 Service Port Number";
static const char_t DCI_LICENSE_INFORMATION_REST_PARAM_DESCRIPTION []= "Open Source License";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_DESCRIPTION []= "IOT Connection status reason enums";
static const char_t DCI_LANG_PREF_SETTING_REST_PARAM_DESCRIPTION []= "Preferred Language Setting";
static const char_t DCI_COMMON_LANG_PREF_REST_PARAM_DESCRIPTION []= "Common Language Preferrence according to ISO 639-1";
static const char_t DCI_DATA_LOG_REST_PARAM_DESCRIPTION []= "Data logging for predefined parameters at fixed interval, change of value or manual trigger";
static const char_t DCI_EVENT_LOG_REST_PARAM_DESCRIPTION []= "Application specific Events Log";
static const char_t DCI_AUDIT_POWER_LOG_REST_PARAM_DESCRIPTION []= "Audit Log for power and reset";
static const char_t DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_DESCRIPTION []= "Audit Log for firmware update";
static const char_t DCI_AUDIT_USER_LOG_REST_PARAM_DESCRIPTION []= "Audit Log for user";
static const char_t DCI_AUDIT_CONFIG_LOG_REST_PARAM_DESCRIPTION []= "Audit Log for configuration";
static const char_t DCI_DEVICE_UP_TIME_SEC_REST_PARAM_DESCRIPTION []= "Device up time since last reboot(in seconds)";
static const char_t DCI_PTP_ENABLE_REST_PARAM_DESCRIPTION []= "Enable or Disable PTP";
static const char_t DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_DESCRIPTION []= "SNTP Time offset max sample in microseconds for validation";
#endif

//*******************************************************
//******     The RESTful Units 
//*******************************************************

static const char_t DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_UNIT[]= "Mbs";
static const char_t DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_UNIT[]= "Mbs";
static const char_t DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_UNIT[]= "";
static const char_t DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_PHY1_AUTONEG_STATE_REST_PARAM_UNIT[]= "";
static const char_t DCI_PHY1_PORT_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_UNIT[]= "";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_UNIT[]= "";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_UNIT[]= "";
static const char_t DCI_ETH_ACD_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_UNIT[]= "";
static const char_t DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_UNIT[]= "";
static const char_t DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_UNIT[]= "";
static const char_t DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_UNIT[]= "";
static const char_t DCI_PRODUCT_SERIAL_NUM_REST_PARAM_UNIT[]= "";
static const char_t DCI_VENDOR_NAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_PRODUCT_CODE_REST_PARAM_UNIT[]= "";
static const char_t DCI_PRODUCT_CODE_ASCII_REST_PARAM_UNIT[]= "";
static const char_t DCI_VENDOR_URL_REST_PARAM_UNIT[]= "";
static const char_t DCI_PRODUCT_NAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_MODEL_NAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_USER_APP_NAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_UNIT[]= "milliseconds";
static const char_t DCI_MULTICAST_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_BROADCAST_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_APP_FIRM_VER_NUM_REST_PARAM_UNIT[]= "";
static const char_t DCI_WEB_FIRM_VER_NUM_REST_PARAM_UNIT[]= "";
static const char_t DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_UNIT[]= "";
static const char_t DCI_ACTIVE_IP_ADDRESS_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_ACTIVE_SUBNET_MASK_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_STATIC_IP_ADDRESS_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_STATIC_SUBNET_MASK_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_UNIT[]= "";
static const char_t DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_UNIT[]= "";
static const char_t DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_UNIT[]= "";
static const char_t DCI_LTK_VER_ASCII_REST_PARAM_UNIT[]= "";
static const char_t DCI_FW_UPGRADE_MODE_REST_PARAM_UNIT[]= "";
static const char_t DCI_USER_WEB_FW_NAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_PRODUCT_PROC_NAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_UNIT[]= "";
static const char_t DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_UNIT[]= "Seconds";
static const char_t DCI_HTTP_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_TRUSTED_IP_WHITELIST_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_UNIX_EPOCH_TIME_REST_PARAM_UNIT[]= "";
static const char_t DCI_TIME_OFFSET_REST_PARAM_UNIT[]= "";
static const char_t DCI_DATE_FORMAT_REST_PARAM_UNIT[]= "";
static const char_t DCI_TIME_FORMAT_REST_PARAM_UNIT[]= "";
static const char_t DCI_REST_RESET_COMMAND_REST_PARAM_UNIT[]= "";
static const char_t DCI_DEVICE_CERT_VALID_REST_PARAM_UNIT[]= "";
static const char_t DCI_SNTP_SERVICE_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_SNTP_SERVER_1_REST_PARAM_UNIT[]= "";
static const char_t DCI_SNTP_SERVER_2_REST_PARAM_UNIT[]= "";
static const char_t DCI_SNTP_SERVER_3_REST_PARAM_UNIT[]= "";
static const char_t DCI_SNTP_UPDATE_TIME_REST_PARAM_UNIT[]= "";
static const char_t DCI_SNTP_RETRY_TIME_REST_PARAM_UNIT[]= "";
static const char_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_UNIT[]= "";
static const char_t DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_UNIT[]= ";S;uS";
static const char_t DCI_IOT_CONNECT_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_STATUS_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_PROXY_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_PROXY_SERVER_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_PROXY_PORT_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_PROXY_USERNAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_PROXY_PASSWORD_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_DEVICE_GUID_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_DEVICE_PROFILE_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_CONN_STRING_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_UNIT[]= "";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_UNIT[]= "Seconds";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_UNIT[]= "Seconds";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_UNIT[]= "Seconds";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_UNIT[]= "Seconds";
static const char_t DCI_LOG_TEST_1_REST_PARAM_UNIT[]= "";
static const char_t DCI_RESET_CAUSE_REST_PARAM_UNIT[]= "";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_UNIT[]= "";
static const char_t DCI_DEVICE_STATE_REST_PARAM_UNIT[]= "";
static const char_t DCI_LOG_EVENT_TEST_101_REST_PARAM_UNIT[]= "";
static const char_t DCI_LOG_EVENT_TEST_102_REST_PARAM_UNIT[]= "";
static const char_t DCI_NV_MEM_ID_REST_PARAM_UNIT[]= "";
static const char_t DCI_NV_PARAMETER_ADDRESS_REST_PARAM_UNIT[]= "";
static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_UNIT[]= "";
static const char_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_UNIT[]= "";
static const char_t DCI_LOG_FIRMWARE_VERSION_REST_PARAM_UNIT[]= "";
static const char_t DCI_LOGGED_USERNAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_LOG_IP_ADDRESS_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_UNIT[]= "";
static const char_t DCI_MODBUS_TCP_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_BACNET_IP_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_UNIT[]= "";
static const char_t DCI_MAX_CONCURRENT_SESSION_REST_PARAM_UNIT[]= "";
static const char_t DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_UNIT[]= "Seconds";
static const char_t DCI_USER_LOCK_TIME_SEC_REST_PARAM_UNIT[]= "Seconds";
static const char_t DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_UNIT[]= "";
static const char_t DCI_CORS_TYPE_REST_PARAM_UNIT[]= "";
static const char_t DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_UNIT[]= "";
static const char_t DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_MDNS_SERVER1_NAME_REST_PARAM_UNIT[]= "";
static const char_t DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_UNIT[]= "IP";
static const char_t DCI_MDNS_SERVER1_PORT_REST_PARAM_UNIT[]= "";
static const char_t DCI_LICENSE_INFORMATION_REST_PARAM_UNIT[]= "";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_UNIT[]= "";
static const char_t DCI_LANG_PREF_SETTING_REST_PARAM_UNIT[]= "";
static const char_t DCI_COMMON_LANG_PREF_REST_PARAM_UNIT[]= "";
static const char_t DCI_DATA_LOG_REST_PARAM_UNIT[]= "";
static const char_t DCI_EVENT_LOG_REST_PARAM_UNIT[]= "";
static const char_t DCI_AUDIT_POWER_LOG_REST_PARAM_UNIT[]= "";
static const char_t DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_UNIT[]= "";
static const char_t DCI_AUDIT_USER_LOG_REST_PARAM_UNIT[]= "";
static const char_t DCI_AUDIT_CONFIG_LOG_REST_PARAM_UNIT[]= "";
static const char_t DCI_DEVICE_UP_TIME_SEC_REST_PARAM_UNIT[]= "Seconds";
static const char_t DCI_PTP_ENABLE_REST_PARAM_UNIT[]= "";
static const char_t DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_UNIT[]= "";


//*******************************************************
//******     The RESTful parameter format names
//*******************************************************

static const char_t FORMAT_NAME_MAC_ADDRESS[]= "MAC_ADDRESS";
static const char_t FORMAT_NAME_IPV4_BIG_ENDIAN_U8[]= "IPV4_BIG_ENDIAN_U8";
static const char_t FORMAT_NAME_WAIVER[]= "WAIVER";
static const char_t FORMAT_NAME_HOST_ADDRESS[]= "HOST_ADDRESS";
static const char_t FORMAT_NAME_CRED[]= "CRED";

//*******************************************************
//******     The RESTful parameter config structures
//*******************************************************

//*******************************************************
//******     The RESTful parameter format structures
//*******************************************************

const formatter_struct_t DCI_ETH_ACD_CONFLICTED_MAC_FORMATTER = 
{
    FORMAT_NAME_MAC_ADDRESS,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_MAC_ADDRESS,
    nullptr
#endif
};

const formatter_struct_t DCI_ETHERNET_MAC_ADDRESS_FORMATTER = 
{
    FORMAT_NAME_MAC_ADDRESS,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_MAC_ADDRESS,
    nullptr
#endif
};

const formatter_struct_t DCI_ACTIVE_IP_ADDRESS_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

const formatter_struct_t DCI_ACTIVE_SUBNET_MASK_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

const formatter_struct_t DCI_ACTIVE_DEFAULT_GATEWAY_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

const formatter_struct_t DCI_STATIC_IP_ADDRESS_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

const formatter_struct_t DCI_STATIC_SUBNET_MASK_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

const formatter_struct_t DCI_STATIC_DEFAULT_GATEWAY_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

const formatter_struct_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_FORMATTER = 
{
    FORMAT_NAME_WAIVER,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_WAIVER,
    nullptr
#endif
};

const formatter_struct_t DCI_TRUSTED_IP_WHITELIST_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

const formatter_struct_t DCI_SNTP_SERVER_1_FORMATTER = 
{
    FORMAT_NAME_HOST_ADDRESS,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_HOST_ADDRESS,
    nullptr
#endif
};

const formatter_struct_t DCI_SNTP_SERVER_2_FORMATTER = 
{
    FORMAT_NAME_HOST_ADDRESS,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_HOST_ADDRESS,
    nullptr
#endif
};

const formatter_struct_t DCI_SNTP_SERVER_3_FORMATTER = 
{
    FORMAT_NAME_HOST_ADDRESS,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_HOST_ADDRESS,
    nullptr
#endif
};

const formatter_struct_t DCI_IOT_PROXY_PASSWORD_FORMATTER = 
{
    FORMAT_NAME_CRED,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_CRED,
    nullptr
#endif
};

const formatter_struct_t DCI_IOT_CONN_STRING_FORMATTER = 
{
    FORMAT_NAME_CRED,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_CRED,
    nullptr
#endif
};

const formatter_struct_t DCI_LOG_IP_ADDRESS_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

const formatter_struct_t DCI_MDNS_SERVER1_IP_ADDR_FORMATTER = 
{
    FORMAT_NAME_IPV4_BIG_ENDIAN_U8,
#ifdef REST_SERVER_FORMAT_HANDLING
    &FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8,
    nullptr
#endif
};

//*******************************************************
//******     The RESTful parameter bitfield descriptions
//*******************************************************

static const char_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_0[]= "Standard power-up";
static const char_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_1[]= "Wakeup from sleep";
static const char_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_2[]= "Watchdog reset";
static const char_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_3[]= "Software - internally";
static const char_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_4[]= "Microcontroller reset pin";
static const char_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_5[]= "Brown out reset";
static const char_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_6[]= "Reset cause undefined";
bitfield_desc_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC[ 7U ]= 
{
    { 0U, DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_0 },
    { 1U, DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_1 },
    { 2U, DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_2 },
    { 3U, DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_3 },
    { 4U, DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_4 },
    { 5U, DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_5 },
    { 6U, DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_VAL_6 }
};
bitfield_desc_block_t DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_BLOCK = 
{
    7U,
    DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC 
};


//*******************************************************
//******     The RESTful parameter enum descriptions
//*******************************************************

static const char_t DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ENUM_DESC_VAL_10[]= "10 Mbps speed";
static const char_t DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ENUM_DESC_VAL_100[]= "100 Mbps speed";
enum_desc_t DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 10U, DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ENUM_DESC_VAL_10 },
    { 100U, DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ENUM_DESC_VAL_100 }
};
enum_desc_block_t DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ENUM_DESC_VAL_1[]= "DIP Switch";
static const char_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ENUM_DESC_VAL_2[]= "Ethernet";
enum_desc_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 1U, DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ENUM_DESC_VAL_2 }
};
enum_desc_block_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_VAL_0[]= "Statically Hardcoded";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_VAL_1[]= "DHCP Allocated";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_VAL_2[]= "Taken From NV";
static const char_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_VAL_3[]= "DIP Switch Configured";
enum_desc_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC[ 4U ]= 
{
    { 0U, DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_VAL_2 },
    { 3U, DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_VAL_3 }
};
enum_desc_block_t DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_BLOCK = 
{
    4U,
    DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_0[]= "This can be accessd without authentication";
static const char_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_7[]= "This is a Role having readonly access";
static const char_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_15[]= "This is an Operator Role having Product control privileges";
static const char_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_31[]= "This is an Engineer Role having Product configuration privileges";
static const char_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_99[]= "This is an Administrator having all privileges";
enum_desc_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC[ 5U ]= 
{
    { 0U, DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_0 },
    { 7U, DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_7 },
    { 15U, DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_15 },
    { 31U, DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_31 },
    { 99U, DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_VAL_99 }
};
enum_desc_block_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_BLOCK = 
{
    5U,
    DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC_VAL_0[]= "Upgrade to any version";
static const char_t DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC_VAL_1[]= "Upgrade to same or higher version";
static const char_t DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC_VAL_2[]= "Upgrade not allowed";
enum_desc_t DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC[ 3U ]= 
{
    { 0U, DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC_VAL_2 }
};
enum_desc_block_t DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    3U,
    DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_HTTP_ENABLE_REST_PARAM_ENUM_DESC_VAL_0[]= "Disable";
static const char_t DCI_HTTP_ENABLE_REST_PARAM_ENUM_DESC_VAL_1[]= "Enable";
enum_desc_t DCI_HTTP_ENABLE_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_HTTP_ENABLE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_HTTP_ENABLE_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_HTTP_ENABLE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_HTTP_ENABLE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_660[]= "GMT-11:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_600[]= "GMT-10:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_540[]= "GMT-09:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_480[]= "GMT-08:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_420[]= "GMT-07:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_360[]= "GMT-06:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_300[]= "GMT-05:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_270[]= "GMT-04:30";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_240[]= "GMT-04:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_210[]= "GMT-03:30";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_180[]= "GMT-03:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_120[]= "GMT-02:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_60[]= "GMT-01:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_0[]= "GMT/UTC";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_60[]= "GMT+01:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_120[]= "GMT+02:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_180[]= "GMT+03:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_210[]= "GMT+03:30";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_240[]= "GMT+04:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_300[]= "GMT+05:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_330[]= "GMT+05:30";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_345[]= "GMT+05:45";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_360[]= "GMT+06:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_390[]= "GMT+06:30";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_420[]= "GMT+07:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_480[]= "GMT+08:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_540[]= "GMT+09:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_570[]= "GMT+09:30";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_600[]= "GMT+10:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_660[]= "GMT+11:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_720[]= "GMT+12:00";
static const char_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_780[]= "GMT+13:00";
enum_desc_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC[ 32U ]= 
{
    { 660U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_660 },
    { 600U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_600 },
    { 540U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_540 },
    { 480U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_480 },
    { 420U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_420 },
    { 360U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_360 },
    { 300U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_300 },
    { 270U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_270 },
    { 240U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_240 },
    { 210U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_210 },
    { 180U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_180 },
    { 120U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_120 },
    { 60U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_60 },
    { 0U, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_0 },
    { -60, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_60 },
    { -120, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_120 },
    { -180, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_180 },
    { -210, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_210 },
    { -240, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_240 },
    { -300, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_300 },
    { -330, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_330 },
    { -345, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_345 },
    { -360, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_360 },
    { -390, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_390 },
    { -420, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_420 },
    { -480, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_480 },
    { -540, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_540 },
    { -570, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_570 },
    { -600, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_600 },
    { -660, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_660 },
    { -720, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_720 },
    { -780, DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_VAL_NEG_780 }
};
enum_desc_block_t DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_BLOCK = 
{
    32U,
    DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_VAL_0[]= "mm/dd/yyyy";
static const char_t DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_VAL_1[]= "dd/mm/yyyy";
static const char_t DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_VAL_2[]= "yyyy-mm-dd";
static const char_t DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_VAL_3[]= "dd mm yyyy";
enum_desc_t DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC[ 4U ]= 
{
    { 0U, DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_VAL_2 },
    { 3U, DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_VAL_3 }
};
enum_desc_block_t DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_BLOCK = 
{
    4U,
    DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_TIME_FORMAT_REST_PARAM_ENUM_DESC_VAL_0[]= "12Hrs (AM/PM)";
static const char_t DCI_TIME_FORMAT_REST_PARAM_ENUM_DESC_VAL_1[]= "24Hrs";
enum_desc_t DCI_TIME_FORMAT_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_TIME_FORMAT_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_TIME_FORMAT_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_TIME_FORMAT_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_TIME_FORMAT_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_REST_RESET_COMMAND_REST_PARAM_ENUM_DESC_VAL_1[]= "Reboot Device";
static const char_t DCI_REST_RESET_COMMAND_REST_PARAM_ENUM_DESC_VAL_2[]= "Factory Reset";
enum_desc_t DCI_REST_RESET_COMMAND_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 1U, DCI_REST_RESET_COMMAND_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_REST_RESET_COMMAND_REST_PARAM_ENUM_DESC_VAL_2 }
};
enum_desc_block_t DCI_REST_RESET_COMMAND_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_REST_RESET_COMMAND_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_DEVICE_CERT_VALID_REST_PARAM_ENUM_DESC_VAL_0[]= "Certificate Invalid";
static const char_t DCI_DEVICE_CERT_VALID_REST_PARAM_ENUM_DESC_VAL_1[]= "Certificate Valid";
enum_desc_t DCI_DEVICE_CERT_VALID_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_DEVICE_CERT_VALID_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_DEVICE_CERT_VALID_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_DEVICE_CERT_VALID_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_DEVICE_CERT_VALID_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ENUM_DESC_VAL_0[]= "Disable";
static const char_t DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ENUM_DESC_VAL_1[]= "Manual Server Entry";
enum_desc_t DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_VAL_0[]= "Unconnected";
static const char_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_VAL_1[]= "Connected_To_Server_1";
static const char_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_VAL_2[]= "Connected_To_Server_2";
static const char_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_VAL_3[]= "Connected_To_Server_3";
enum_desc_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC[ 4U ]= 
{
    { 0U, DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_VAL_2 },
    { 3U, DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_VAL_3 }
};
enum_desc_block_t DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_BLOCK = 
{
    4U,
    DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_5[]= "5 seconds";
static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_10[]= "10 seconds";
static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_15[]= "15 seconds";
static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_20[]= "20 seconds";
static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_25[]= "25 seconds";
static const char_t DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_30[]= "30 seconds";
enum_desc_t DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC[ 6U ]= 
{
    { 5U, DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_5 },
    { 10U, DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_10 },
    { 15U, DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_15 },
    { 20U, DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_20 },
    { 25U, DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_25 },
    { 30U, DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_30 }
};
enum_desc_block_t DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    6U,
    DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_5[]= "5 seconds";
static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_10[]= "10 seconds";
static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_15[]= "15 seconds";
static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_20[]= "20 seconds";
static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_25[]= "25 seconds";
static const char_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_30[]= "30 seconds";
enum_desc_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC[ 6U ]= 
{
    { 5U, DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_5 },
    { 10U, DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_10 },
    { 15U, DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_15 },
    { 20U, DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_20 },
    { 25U, DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_25 },
    { 30U, DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_VAL_30 }
};
enum_desc_block_t DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    6U,
    DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_NEG_1[]= "Stop publishing";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_0[]= "Immediate publishing";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_1000[]= "Cadence 1s";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_5000[]= "Cadence 5s";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_10000[]= "Cadence 10s";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_15000[]= "Cadence 15s";
static const char_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_20000[]= "Cadence 20s";
enum_desc_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC[ 7U ]= 
{
    { -1, DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_NEG_1 },
    { 0U, DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1000U, DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_1000 },
    { 5000U, DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_5000 },
    { 10000U, DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_10000 },
    { 15000U, DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_15000 },
    { 20000U, DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_VAL_20000 }
};
enum_desc_block_t DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    7U,
    DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_NEG_1[]= "Stop publishing";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_10000[]= "Cadence_10s";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_60000[]= "Cadence_60s";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_120000[]= "Cadence_120s";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_300000[]= "Cadence_300s";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_500000[]= "Cadence_500s";
static const char_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_600000[]= "Cadence_600s";
enum_desc_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC[ 7U ]= 
{
    { -1, DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_NEG_1 },
    { 10000U, DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_10000 },
    { 60000U, DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_60000 },
    { 120000U, DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_120000 },
    { 300000U, DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_300000 },
    { 500000U, DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_500000 },
    { 600000U, DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_VAL_600000 }
};
enum_desc_block_t DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    7U,
    DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_NEG_1[]= "Stop publishing";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_0[]= "Immediate publishing";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_1000[]= "Cadence 1s";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_5000[]= "Cadence 5s";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_10000[]= "Cadence 10s";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_15000[]= "Cadence 15s";
static const char_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_20000[]= "Cadence 20s";
enum_desc_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC[ 7U ]= 
{
    { -1, DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_NEG_1 },
    { 0U, DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1000U, DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_1000 },
    { 5000U, DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_5000 },
    { 10000U, DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_10000 },
    { 15000U, DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_15000 },
    { 20000U, DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_VAL_20000 }
};
enum_desc_block_t DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    7U,
    DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_NEG_1[]= "Stop publishing";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_0[]= "Immediate publishing";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_1000[]= "Cadence 1s";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_5000[]= "Cadence 5s";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_10000[]= "Cadence 10s";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_15000[]= "Cadence 15s";
static const char_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_20000[]= "Cadence 20s";
enum_desc_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC[ 7U ]= 
{
    { -1, DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_NEG_1 },
    { 0U, DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1000U, DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_1000 },
    { 5000U, DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_5000 },
    { 10000U, DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_10000 },
    { 15000U, DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_15000 },
    { 20000U, DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_VAL_20000 }
};
enum_desc_block_t DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    7U,
    DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_0[]= "No Event";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_1[]= "Device Restart";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_2[]= "Factory Reset";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_3[]= "Image upgrade started";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_4[]= "Image upgrade complete";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_5[]= "Checksum failure";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_6[]= "Signature verification failure";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_7[]= "Version rollback failure";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_8[]= "NV data restore fail";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_9[]= "NV param write fail";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_10[]= "NV parameter erase fail";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_11[]= "NV erase all parameters fail";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_12[]= "Self user password change";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_13[]= "Other user password change";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_14[]= "User created";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_15[]= "User deleted";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_16[]= "Admin user logged event";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_17[]= "User logged in event";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_18[]= "User logout";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_19[]= "User Locked event";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_20[]= "Reset default admin user";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_21[]= "Reset all user accounts";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_22[]= "Data Log Memory Clear";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_23[]= "Event Log Memory Clear";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_24[]= "Audit-Power Log Memory Clear";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_25[]= "Audit-Fw-Update Log Memory Clear";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_26[]= "Audit-User Log Memory Clear";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_27[]= "Audit-Config Log Memory Clear";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_28[]= "Image upgrade started over IOT";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_29[]= "Image upgrade completed over IOT";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_30[]= "Image upgrade aborted/cancelled over IOT";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_31[]= "Image upgrade failed over IOT due to bad parameter";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_32[]= "Checksum failure over IOT";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_33[]= "Signature verification failure over IOT";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_34[]= "Version rollback failure over IOT";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_35[]= "IOT FUS request rejected on product side";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_101[]= "Test event-101";
static const char_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_102[]= "Test event-102";
enum_desc_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC[ 38U ]= 
{
    { 0U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_2 },
    { 3U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_3 },
    { 4U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_4 },
    { 5U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_5 },
    { 6U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_6 },
    { 7U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_7 },
    { 8U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_8 },
    { 9U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_9 },
    { 10U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_10 },
    { 11U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_11 },
    { 12U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_12 },
    { 13U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_13 },
    { 14U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_14 },
    { 15U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_15 },
    { 16U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_16 },
    { 17U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_17 },
    { 18U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_18 },
    { 19U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_19 },
    { 20U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_20 },
    { 21U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_21 },
    { 22U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_22 },
    { 23U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_23 },
    { 24U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_24 },
    { 25U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_25 },
    { 26U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_26 },
    { 27U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_27 },
    { 28U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_28 },
    { 29U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_29 },
    { 30U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_30 },
    { 31U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_31 },
    { 32U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_32 },
    { 33U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_33 },
    { 34U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_34 },
    { 35U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_35 },
    { 101U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_101 },
    { 102U, DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_VAL_102 }
};
enum_desc_block_t DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    38U,
    DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_DEVICE_STATE_REST_PARAM_ENUM_DESC_VAL_0[]= "Operational";
static const char_t DCI_DEVICE_STATE_REST_PARAM_ENUM_DESC_VAL_1[]= "Fault";
enum_desc_t DCI_DEVICE_STATE_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_DEVICE_STATE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_DEVICE_STATE_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_DEVICE_STATE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_DEVICE_STATE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_NV_MEM_ID_REST_PARAM_ENUM_DESC_VAL_0[]= "FRAM";
static const char_t DCI_NV_MEM_ID_REST_PARAM_ENUM_DESC_VAL_1[]= "Undefined";
enum_desc_t DCI_NV_MEM_ID_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_NV_MEM_ID_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_NV_MEM_ID_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_NV_MEM_ID_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_NV_MEM_ID_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_0[]= "Initilization";
static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_1[]= "Read Parameter";
static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_2[]= "Read Checksum";
static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_3[]= "Write Parameter";
static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_4[]= "Erase Parameter";
static const char_t DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_5[]= "Erase All Parameters";
enum_desc_t DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC[ 6U ]= 
{
    { 0U, DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_2 },
    { 3U, DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_3 },
    { 4U, DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_4 },
    { 5U, DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_VAL_5 }
};
enum_desc_block_t DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_BLOCK = 
{
    6U,
    DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC_VAL_0[]= "Processor Main and Image APPLICATION";
static const char_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC_VAL_1[]= "Processor Main and Image WEB";
static const char_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC_VAL_2[]= "Processor Main and Image LANGUAGE";
enum_desc_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC[ 3U ]= 
{
    { 0U, DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC_VAL_2 }
};
enum_desc_block_t DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC_BLOCK = 
{
    3U,
    DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_0[]= "Unknown Port Value";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_1[]= "HTTP Port";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_2[]= "HTTPS Port";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_3[]= "MODBUS TCP Port";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_4[]= "BACNET Port";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_5[]= "BLE Port";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_6[]= "Ethernet IP over TCP Port";
static const char_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_7[]= "Ethernet IP over UDP Port";
enum_desc_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC[ 8U ]= 
{
    { 0U, DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_2 },
    { 3U, DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_3 },
    { 4U, DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_4 },
    { 5U, DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_5 },
    { 6U, DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_6 },
    { 7U, DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_VAL_7 }
};
enum_desc_block_t DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_BLOCK = 
{
    8U,
    DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_MODBUS_TCP_ENABLE_REST_PARAM_ENUM_DESC_VAL_0[]= "Disable";
static const char_t DCI_MODBUS_TCP_ENABLE_REST_PARAM_ENUM_DESC_VAL_1[]= "Enable";
enum_desc_t DCI_MODBUS_TCP_ENABLE_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_MODBUS_TCP_ENABLE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_MODBUS_TCP_ENABLE_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_MODBUS_TCP_ENABLE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_MODBUS_TCP_ENABLE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_BACNET_IP_ENABLE_REST_PARAM_ENUM_DESC_VAL_0[]= "Disable";
static const char_t DCI_BACNET_IP_ENABLE_REST_PARAM_ENUM_DESC_VAL_1[]= "Enable";
enum_desc_t DCI_BACNET_IP_ENABLE_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_BACNET_IP_ENABLE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_BACNET_IP_ENABLE_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_BACNET_IP_ENABLE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_BACNET_IP_ENABLE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_CORS_TYPE_REST_PARAM_ENUM_DESC_VAL_0[]= "(*) - allows all Origin";
static const char_t DCI_CORS_TYPE_REST_PARAM_ENUM_DESC_VAL_1[]= "(Active device IP) - allows Origin with device IP";
enum_desc_t DCI_CORS_TYPE_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_CORS_TYPE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_CORS_TYPE_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_CORS_TYPE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_CORS_TYPE_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC_VAL_0[]= "Certificate Generation Disable. If disabled certificate will NOT generate after factory reset.";
static const char_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC_VAL_1[]= "Certificate Generation Enable. If enabled certificate will generate after factory reset.";
static const char_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC_VAL_2[]= "Certificate Generation Enable on IP change. If enabled certificate will generate on boot-up if IP is changed.";
enum_desc_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC[ 3U ]= 
{
    { 0U, DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC_VAL_2 }
};
enum_desc_block_t DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC_BLOCK = 
{
    3U,
    DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_0[]= "IoT initial state (default)";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_1[]= "Cloud connected";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_2[]= "IoT disabled";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_3[]= "IoT clock not correctly set";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_4[]= "Reconnecting";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_5[]= "Failed to open connection";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_6[]= "Too many lost messages";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_7[]= "Invalid connection string";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_8[]= "Invalid configuration";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_9[]= "Daily cloud message limit reached";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_10[]= "SAS token expired";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_11[]= "Device disabled by user on IoT hub";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_12[]= "Retry expired";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_13[]= "No network";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_14[]= "Communication error";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_15[]= "Unknown error";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_16[]= "TLS initialization failure";
static const char_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_17[]= "Ethernet/network interface link down";
enum_desc_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC[ 18U ]= 
{
    { 0U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_2 },
    { 3U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_3 },
    { 4U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_4 },
    { 5U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_5 },
    { 6U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_6 },
    { 7U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_7 },
    { 8U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_8 },
    { 9U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_9 },
    { 10U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_10 },
    { 11U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_11 },
    { 12U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_12 },
    { 13U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_13 },
    { 14U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_14 },
    { 15U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_15 },
    { 16U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_16 },
    { 17U, DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_VAL_17 }
};
enum_desc_block_t DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_BLOCK = 
{
    18U,
    DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_VAL_0[]= "Use Device stored language setting";
static const char_t DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_VAL_1[]= "Use Device stored language setting per user";
static const char_t DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_VAL_2[]= "Use browser stored language setting";
static const char_t DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_VAL_3[]= "Use browser local language setting";
enum_desc_t DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC[ 4U ]= 
{
    { 0U, DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_VAL_1 },
    { 2U, DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_VAL_2 },
    { 3U, DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_VAL_3 }
};
enum_desc_block_t DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_BLOCK = 
{
    4U,
    DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC 
};

static const char_t DCI_PTP_ENABLE_REST_PARAM_ENUM_DESC_VAL_0[]= "PTP disable";
static const char_t DCI_PTP_ENABLE_REST_PARAM_ENUM_DESC_VAL_1[]= "PTP enable";
enum_desc_t DCI_PTP_ENABLE_REST_PARAM_ENUM_DESC[ 2U ]= 
{
    { 0U, DCI_PTP_ENABLE_REST_PARAM_ENUM_DESC_VAL_0 },
    { 1U, DCI_PTP_ENABLE_REST_PARAM_ENUM_DESC_VAL_1 }
};
enum_desc_block_t DCI_PTP_ENABLE_REST_PARAM_ENUM_DESC_BLOCK = 
{
    2U,
    DCI_PTP_ENABLE_REST_PARAM_ENUM_DESC 
};


//*******************************************************
//******     The Target DCI to RESTful parameters Structure.
//*******************************************************

static const DCI_REST_TO_DCID_LKUP_ST_TD base_dci_rest_data_param_list[NUM_TOTAL_REST_DCI_DATA_PARAMS] =
{
    {
        // Ethernet PHY 1 - Link Speed Select
        DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ID,
        DCI_PHY1_LINK_SPEED_SELECT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_DESCRIPTION,
#endif
        DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_UNIT,
        nullptr,
        &DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PHY1_LINK_SPEED_SELECT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Ethernet PHY 1 - Link Speed Actual
        DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_ID,
        DCI_PHY1_LINK_SPEED_ACTUAL_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_DESCRIPTION,
#endif
        DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PHY1_LINK_SPEED_ACTUAL_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Ethernet PHY 1 - Duplex Select
        DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_ID,
        DCI_PHY1_FULL_DUPLEX_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PHY1_FULL_DUPLEX_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Ethernet PHY 1 - Duplex Actual
        DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_ID,
        DCI_PHY1_FULL_DUPLEX_ACTUAL_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_DESCRIPTION,
#endif
        DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PHY1_FULL_DUPLEX_ACTUAL_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Ethernet PHY 1 - Link Auto-Negotiate Enable
        DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_ID,
        DCI_PHY1_AUTONEG_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PHY1_AUTONEG_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Ethernet PHY 1 - Link Auto-Negotiate State
        DCI_PHY1_AUTONEG_STATE_REST_PARAM_ID,
        DCI_PHY1_AUTONEG_STATE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PHY1_AUTONEG_STATE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PHY1_AUTONEG_STATE_REST_PARAM_DESCRIPTION,
#endif
        DCI_PHY1_AUTONEG_STATE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PHY1_AUTONEG_STATE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PHY1_AUTONEG_STATE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PHY1_AUTONEG_STATE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Ethernet PHY 1 - Port Enable
        DCI_PHY1_PORT_ENABLE_REST_PARAM_ID,
        DCI_PHY1_PORT_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PHY1_PORT_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PHY1_PORT_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_PHY1_PORT_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PHY1_PORT_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PHY1_PORT_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PHY1_PORT_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Status of Control of IP Address Allocation 
        DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ID,
        DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_DESCRIPTION,
#endif
        DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_UNIT,
        nullptr,
        &DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Shows the present IP Address Allocation Method
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_ID,
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_DESCRIPTION,
#endif
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Allows to set IP Address Allocation Method
        DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ID,
        DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_DESCRIPTION,
#endif
        DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_UNIT,
        nullptr,
        &DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // ACD Enable
        DCI_ETH_ACD_ENABLE_REST_PARAM_ID,
        DCI_ETH_ACD_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ETH_ACD_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ETH_ACD_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_ETH_ACD_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ETH_ACD_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // ACD Conflict State
        DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_ID,
        DCI_ETH_ACD_CONFLICT_STATE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_DESCRIPTION,
#endif
        DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ETH_ACD_CONFLICT_STATE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // ACD Conflicted State
        DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_ID,
        DCI_ETH_ACD_CONFLICTED_STATE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_DESCRIPTION,
#endif
        DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ETH_ACD_CONFLICTED_STATE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // ACD Conflicted Device MAC
        DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_ID,
        DCI_ETH_ACD_CONFLICTED_MAC_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_DESCRIPTION,
#endif
        DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_ETH_ACD_CONFLICTED_MAC_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ETH_ACD_CONFLICTED_MAC_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // ACD ARP PDU Conflicted Message
        DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_ID,
        DCI_ETH_ACD_CONFLICTED_ARP_PDU_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_DESCRIPTION,
#endif
        DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ETH_ACD_CONFLICTED_ARP_PDU_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Device Product Serial Number
        DCI_PRODUCT_SERIAL_NUM_REST_PARAM_ID,
        DCI_PRODUCT_SERIAL_NUM_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PRODUCT_SERIAL_NUM_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PRODUCT_SERIAL_NUM_REST_PARAM_DESCRIPTION,
#endif
        DCI_PRODUCT_SERIAL_NUM_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_SERIAL_NUM_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_SERIAL_NUM_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PRODUCT_SERIAL_NUM_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Product Vendor Name
        DCI_VENDOR_NAME_REST_PARAM_ID,
        DCI_VENDOR_NAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_VENDOR_NAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_VENDOR_NAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_VENDOR_NAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_VENDOR_NAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_VENDOR_NAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_VENDOR_NAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Product Code
        DCI_PRODUCT_CODE_REST_PARAM_ID,
        DCI_PRODUCT_CODE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PRODUCT_CODE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PRODUCT_CODE_REST_PARAM_DESCRIPTION,
#endif
        DCI_PRODUCT_CODE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_CODE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_CODE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PRODUCT_CODE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Communication Device Product Code in ASCII
        DCI_PRODUCT_CODE_ASCII_REST_PARAM_ID,
        DCI_PRODUCT_CODE_ASCII_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PRODUCT_CODE_ASCII_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PRODUCT_CODE_ASCII_REST_PARAM_DESCRIPTION,
#endif
        DCI_PRODUCT_CODE_ASCII_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_CODE_ASCII_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_CODE_ASCII_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PRODUCT_CODE_ASCII_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Corporate URL
        DCI_VENDOR_URL_REST_PARAM_ID,
        DCI_VENDOR_URL_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_VENDOR_URL_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_VENDOR_URL_REST_PARAM_DESCRIPTION,
#endif
        DCI_VENDOR_URL_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_VENDOR_URL_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_VENDOR_URL_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_VENDOR_URL_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Product Name - Short description of Prod Code
        DCI_PRODUCT_NAME_REST_PARAM_ID,
        DCI_PRODUCT_NAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PRODUCT_NAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PRODUCT_NAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_PRODUCT_NAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_NAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_NAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PRODUCT_NAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Model Name
        DCI_MODEL_NAME_REST_PARAM_ID,
        DCI_MODEL_NAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MODEL_NAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MODEL_NAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_MODEL_NAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MODEL_NAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MODEL_NAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MODEL_NAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // User Application Name or Device Tag
        DCI_USER_APP_NAME_REST_PARAM_ID,
        DCI_USER_APP_NAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_USER_APP_NAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_USER_APP_NAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_USER_APP_NAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_USER_APP_NAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_USER_APP_NAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_USER_APP_NAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Modbus Comm Loss Timeout Value (ms)
        DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_ID,
        DCI_MODBUS_TCP_COMM_TIMEOUT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_DESCRIPTION,
#endif
        DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Multicast settings
        DCI_MULTICAST_ENABLE_REST_PARAM_ID,
        DCI_MULTICAST_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MULTICAST_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MULTICAST_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_MULTICAST_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MULTICAST_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MULTICAST_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MULTICAST_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Broadcast settings
        DCI_BROADCAST_ENABLE_REST_PARAM_ID,
        DCI_BROADCAST_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_BROADCAST_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_BROADCAST_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_BROADCAST_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_BROADCAST_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_BROADCAST_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_BROADCAST_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Device Firmware Revision Number
        DCI_APP_FIRM_VER_NUM_REST_PARAM_ID,
        DCI_APP_FIRM_VER_NUM_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_APP_FIRM_VER_NUM_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_APP_FIRM_VER_NUM_REST_PARAM_DESCRIPTION,
#endif
        DCI_APP_FIRM_VER_NUM_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_APP_FIRM_VER_NUM_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_APP_FIRM_VER_NUM_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_APP_FIRM_VER_NUM_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Web Firmware Version Number
        DCI_WEB_FIRM_VER_NUM_REST_PARAM_ID,
        DCI_WEB_FIRM_VER_NUM_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_WEB_FIRM_VER_NUM_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_WEB_FIRM_VER_NUM_REST_PARAM_DESCRIPTION,
#endif
        DCI_WEB_FIRM_VER_NUM_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_WEB_FIRM_VER_NUM_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_WEB_FIRM_VER_NUM_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_WEB_FIRM_VER_NUM_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Ethernet MAC Address
        DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_ID,
        DCI_ETHERNET_MAC_ADDRESS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_DESCRIPTION,
#endif
        DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_ETHERNET_MAC_ADDRESS_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Active IP Address
        DCI_ACTIVE_IP_ADDRESS_REST_PARAM_ID,
        DCI_ACTIVE_IP_ADDRESS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ACTIVE_IP_ADDRESS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ACTIVE_IP_ADDRESS_REST_PARAM_DESCRIPTION,
#endif
        DCI_ACTIVE_IP_ADDRESS_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_ACTIVE_IP_ADDRESS_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ACTIVE_IP_ADDRESS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ACTIVE_IP_ADDRESS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ACTIVE_IP_ADDRESS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Active Subnet Mask
        DCI_ACTIVE_SUBNET_MASK_REST_PARAM_ID,
        DCI_ACTIVE_SUBNET_MASK_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ACTIVE_SUBNET_MASK_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ACTIVE_SUBNET_MASK_REST_PARAM_DESCRIPTION,
#endif
        DCI_ACTIVE_SUBNET_MASK_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_ACTIVE_SUBNET_MASK_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ACTIVE_SUBNET_MASK_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ACTIVE_SUBNET_MASK_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ACTIVE_SUBNET_MASK_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Active Default Gateway
        DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_ID,
        DCI_ACTIVE_DEFAULT_GATEWAY_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_DESCRIPTION,
#endif
        DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_ACTIVE_DEFAULT_GATEWAY_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Static IP Address
        DCI_STATIC_IP_ADDRESS_REST_PARAM_ID,
        DCI_STATIC_IP_ADDRESS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_STATIC_IP_ADDRESS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_STATIC_IP_ADDRESS_REST_PARAM_DESCRIPTION,
#endif
        DCI_STATIC_IP_ADDRESS_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_STATIC_IP_ADDRESS_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_STATIC_IP_ADDRESS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_STATIC_IP_ADDRESS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_STATIC_IP_ADDRESS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Static Subnet Mask
        DCI_STATIC_SUBNET_MASK_REST_PARAM_ID,
        DCI_STATIC_SUBNET_MASK_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_STATIC_SUBNET_MASK_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_STATIC_SUBNET_MASK_REST_PARAM_DESCRIPTION,
#endif
        DCI_STATIC_SUBNET_MASK_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_STATIC_SUBNET_MASK_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_STATIC_SUBNET_MASK_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_STATIC_SUBNET_MASK_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_STATIC_SUBNET_MASK_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Static Default Gateway
        DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_ID,
        DCI_STATIC_DEFAULT_GATEWAY_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_DESCRIPTION,
#endif
        DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_STATIC_DEFAULT_GATEWAY_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // Web Access Password Waiver Level
        DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ID,
        DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_DESCRIPTION,
#endif
        DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_UNIT,
        nullptr,
        &DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_ENUM_DESC_BLOCK,
        &DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_REST_PARAM_DESCRIPTION)) -1U),
#endif
        0U,
        99U
    },
    {
        // simple application control word
        DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_ID,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_DESCRIPTION,
#endif
        DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SIMPLE_APP_CONTROL_WORD_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // simple application status word
        DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_ID,
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_DESCRIPTION,
#endif
        DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SIMPLE_APP_STATUS_WORD_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        31U
    },
    {
        // LTK Revision ASCII
        DCI_LTK_VER_ASCII_REST_PARAM_ID,
        DCI_LTK_VER_ASCII_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LTK_VER_ASCII_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LTK_VER_ASCII_REST_PARAM_DESCRIPTION,
#endif
        DCI_LTK_VER_ASCII_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LTK_VER_ASCII_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LTK_VER_ASCII_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LTK_VER_ASCII_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Firmware Upgrade Mode
        DCI_FW_UPGRADE_MODE_REST_PARAM_ID,
        DCI_FW_UPGRADE_MODE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_FW_UPGRADE_MODE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_FW_UPGRADE_MODE_REST_PARAM_DESCRIPTION,
#endif
        DCI_FW_UPGRADE_MODE_REST_PARAM_UNIT,
        nullptr,
        &DCI_FW_UPGRADE_MODE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_FW_UPGRADE_MODE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_FW_UPGRADE_MODE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_FW_UPGRADE_MODE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Product Web Firmware Name
        DCI_USER_WEB_FW_NAME_REST_PARAM_ID,
        DCI_USER_WEB_FW_NAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_USER_WEB_FW_NAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_USER_WEB_FW_NAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_USER_WEB_FW_NAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_USER_WEB_FW_NAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_USER_WEB_FW_NAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_USER_WEB_FW_NAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Product proce Name - Short description of Prod Code
        DCI_PRODUCT_PROC_NAME_REST_PARAM_ID,
        DCI_PRODUCT_PROC_NAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PRODUCT_PROC_NAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PRODUCT_PROC_NAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_PRODUCT_PROC_NAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_PROC_NAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_PROC_NAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PRODUCT_PROC_NAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Device Product proc Serial Number
        DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_ID,
        DCI_PRODUCT_PROC_SERIAL_NUM_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_DESCRIPTION,
#endif
        DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PRODUCT_PROC_SERIAL_NUM_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // User account Inactivity timeout
        DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_ID,
        DCI_USER_INACTIVITY_TIMEOUT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_DESCRIPTION,
#endif
        DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_USER_INACTIVITY_TIMEOUT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // HTTP enable or disable
        DCI_HTTP_ENABLE_REST_PARAM_ID,
        DCI_HTTP_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_HTTP_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_HTTP_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_HTTP_ENABLE_REST_PARAM_UNIT,
        nullptr,
        &DCI_HTTP_ENABLE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_HTTP_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_HTTP_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_HTTP_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Trusted IP Adress filter white list Modbus/TCP
        DCI_TRUSTED_IP_WHITELIST_REST_PARAM_ID,
        DCI_TRUSTED_IP_WHITELIST_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_TRUSTED_IP_WHITELIST_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_TRUSTED_IP_WHITELIST_REST_PARAM_DESCRIPTION,
#endif
        DCI_TRUSTED_IP_WHITELIST_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_TRUSTED_IP_WHITELIST_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_TRUSTED_IP_WHITELIST_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_TRUSTED_IP_WHITELIST_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_TRUSTED_IP_WHITELIST_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Trusted IP filter for Modbus/TCP Enable
        DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_ID,
        DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Unix Epoch Time
        DCI_UNIX_EPOCH_TIME_REST_PARAM_ID,
        DCI_UNIX_EPOCH_TIME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_UNIX_EPOCH_TIME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_UNIX_EPOCH_TIME_REST_PARAM_DESCRIPTION,
#endif
        DCI_UNIX_EPOCH_TIME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_UNIX_EPOCH_TIME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_UNIX_EPOCH_TIME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_UNIX_EPOCH_TIME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Time Offset
        DCI_TIME_OFFSET_REST_PARAM_ID,
        DCI_TIME_OFFSET_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_TIME_OFFSET_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_TIME_OFFSET_REST_PARAM_DESCRIPTION,
#endif
        DCI_TIME_OFFSET_REST_PARAM_UNIT,
        nullptr,
        &DCI_TIME_OFFSET_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_TIME_OFFSET_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_TIME_OFFSET_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_TIME_OFFSET_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Date Format
        DCI_DATE_FORMAT_REST_PARAM_ID,
        DCI_DATE_FORMAT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_DATE_FORMAT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_DATE_FORMAT_REST_PARAM_DESCRIPTION,
#endif
        DCI_DATE_FORMAT_REST_PARAM_UNIT,
        nullptr,
        &DCI_DATE_FORMAT_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_DATE_FORMAT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_DATE_FORMAT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_DATE_FORMAT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Time Format
        DCI_TIME_FORMAT_REST_PARAM_ID,
        DCI_TIME_FORMAT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_TIME_FORMAT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_TIME_FORMAT_REST_PARAM_DESCRIPTION,
#endif
        DCI_TIME_FORMAT_REST_PARAM_UNIT,
        nullptr,
        &DCI_TIME_FORMAT_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_TIME_FORMAT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_TIME_FORMAT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_TIME_FORMAT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // REST Reset Command
        DCI_REST_RESET_COMMAND_REST_PARAM_ID,
        DCI_REST_RESET_COMMAND_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_REST_RESET_COMMAND_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_REST_RESET_COMMAND_REST_PARAM_DESCRIPTION,
#endif
        DCI_REST_RESET_COMMAND_REST_PARAM_UNIT,
        nullptr,
        &DCI_REST_RESET_COMMAND_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_REST_RESET_COMMAND_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_REST_RESET_COMMAND_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_REST_RESET_COMMAND_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Device Certificate Validity Status
        DCI_DEVICE_CERT_VALID_REST_PARAM_ID,
        DCI_DEVICE_CERT_VALID_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_DEVICE_CERT_VALID_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_DEVICE_CERT_VALID_REST_PARAM_DESCRIPTION,
#endif
        DCI_DEVICE_CERT_VALID_REST_PARAM_UNIT,
        nullptr,
        &DCI_DEVICE_CERT_VALID_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_DEVICE_CERT_VALID_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_DEVICE_CERT_VALID_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_DEVICE_CERT_VALID_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Sntp Operation Enable
        DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ID,
        DCI_SNTP_SERVICE_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SNTP_SERVICE_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SNTP_SERVICE_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_SNTP_SERVICE_ENABLE_REST_PARAM_UNIT,
        nullptr,
        &DCI_SNTP_SERVICE_ENABLE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SNTP_SERVICE_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SNTP_SERVICE_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SNTP_SERVICE_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Time Server 1
        DCI_SNTP_SERVER_1_REST_PARAM_ID,
        DCI_SNTP_SERVER_1_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SNTP_SERVER_1_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SNTP_SERVER_1_REST_PARAM_DESCRIPTION,
#endif
        DCI_SNTP_SERVER_1_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_SNTP_SERVER_1_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SNTP_SERVER_1_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SNTP_SERVER_1_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SNTP_SERVER_1_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Time Server 2
        DCI_SNTP_SERVER_2_REST_PARAM_ID,
        DCI_SNTP_SERVER_2_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SNTP_SERVER_2_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SNTP_SERVER_2_REST_PARAM_DESCRIPTION,
#endif
        DCI_SNTP_SERVER_2_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_SNTP_SERVER_2_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SNTP_SERVER_2_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SNTP_SERVER_2_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SNTP_SERVER_2_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Time Server 3
        DCI_SNTP_SERVER_3_REST_PARAM_ID,
        DCI_SNTP_SERVER_3_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SNTP_SERVER_3_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SNTP_SERVER_3_REST_PARAM_DESCRIPTION,
#endif
        DCI_SNTP_SERVER_3_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_SNTP_SERVER_3_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SNTP_SERVER_3_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SNTP_SERVER_3_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SNTP_SERVER_3_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Sntp Update Time
        DCI_SNTP_UPDATE_TIME_REST_PARAM_ID,
        DCI_SNTP_UPDATE_TIME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SNTP_UPDATE_TIME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SNTP_UPDATE_TIME_REST_PARAM_DESCRIPTION,
#endif
        DCI_SNTP_UPDATE_TIME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SNTP_UPDATE_TIME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SNTP_UPDATE_TIME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SNTP_UPDATE_TIME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Sntp Retry Time
        DCI_SNTP_RETRY_TIME_REST_PARAM_ID,
        DCI_SNTP_RETRY_TIME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SNTP_RETRY_TIME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SNTP_RETRY_TIME_REST_PARAM_DESCRIPTION,
#endif
        DCI_SNTP_RETRY_TIME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SNTP_RETRY_TIME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SNTP_RETRY_TIME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SNTP_RETRY_TIME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Sntp Active Server Status
        DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ID,
        DCI_SNTP_ACTIVE_SERVER_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SNTP_ACTIVE_SERVER_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SNTP_ACTIVE_SERVER_REST_PARAM_DESCRIPTION,
#endif
        DCI_SNTP_ACTIVE_SERVER_REST_PARAM_UNIT,
        nullptr,
        &DCI_SNTP_ACTIVE_SERVER_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SNTP_ACTIVE_SERVER_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SNTP_ACTIVE_SERVER_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SNTP_ACTIVE_SERVER_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Unix Epoch Time_64bit
        DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_ID,
        DCI_UNIX_EPOCH_TIME_64_BIT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_DESCRIPTION,
#endif
        DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_UNIX_EPOCH_TIME_64_BIT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // IOT Enable or Disable
        DCI_IOT_CONNECT_REST_PARAM_ID,
        DCI_IOT_CONNECT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_CONNECT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_CONNECT_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_CONNECT_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_CONNECT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_CONNECT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_CONNECT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // IOT Connection Status
        DCI_IOT_STATUS_REST_PARAM_ID,
        DCI_IOT_STATUS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_STATUS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_STATUS_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_STATUS_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_STATUS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_STATUS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_STATUS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Proxy Enable 
        DCI_IOT_PROXY_ENABLE_REST_PARAM_ID,
        DCI_IOT_PROXY_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_PROXY_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_PROXY_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_PROXY_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_PROXY_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Proxy Server Address
        DCI_IOT_PROXY_SERVER_REST_PARAM_ID,
        DCI_IOT_PROXY_SERVER_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_PROXY_SERVER_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_PROXY_SERVER_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_PROXY_SERVER_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_SERVER_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_SERVER_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_PROXY_SERVER_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Proxy Server Port
        DCI_IOT_PROXY_PORT_REST_PARAM_ID,
        DCI_IOT_PROXY_PORT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_PROXY_PORT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_PROXY_PORT_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_PROXY_PORT_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_PORT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_PORT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_PROXY_PORT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Proxy Username
        DCI_IOT_PROXY_USERNAME_REST_PARAM_ID,
        DCI_IOT_PROXY_USERNAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_PROXY_USERNAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_PROXY_USERNAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_PROXY_USERNAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_USERNAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_USERNAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_PROXY_USERNAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Proxy Password
        DCI_IOT_PROXY_PASSWORD_REST_PARAM_ID,
        DCI_IOT_PROXY_PASSWORD_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_PROXY_PASSWORD_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_PROXY_PASSWORD_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_PROXY_PASSWORD_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_IOT_PROXY_PASSWORD_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_PASSWORD_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_PROXY_PASSWORD_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_PROXY_PASSWORD_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Device GUID
        DCI_IOT_DEVICE_GUID_REST_PARAM_ID,
        DCI_IOT_DEVICE_GUID_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_DEVICE_GUID_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_DEVICE_GUID_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_DEVICE_GUID_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_DEVICE_GUID_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_DEVICE_GUID_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_DEVICE_GUID_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Device Profile
        DCI_IOT_DEVICE_PROFILE_REST_PARAM_ID,
        DCI_IOT_DEVICE_PROFILE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_DEVICE_PROFILE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_DEVICE_PROFILE_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_DEVICE_PROFILE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_DEVICE_PROFILE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_DEVICE_PROFILE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_DEVICE_PROFILE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // IOT Hub Server Connection string 
        DCI_IOT_CONN_STRING_REST_PARAM_ID,
        DCI_IOT_CONN_STRING_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_CONN_STRING_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_CONN_STRING_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_CONN_STRING_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_IOT_CONN_STRING_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_CONN_STRING_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_CONN_STRING_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_CONN_STRING_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // IOT Data Update Rate
        DCI_IOT_UPDATE_RATE_REST_PARAM_ID,
        DCI_IOT_UPDATE_RATE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_UPDATE_RATE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_UPDATE_RATE_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_UPDATE_RATE_REST_PARAM_UNIT,
        nullptr,
        &DCI_IOT_UPDATE_RATE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_UPDATE_RATE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_UPDATE_RATE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_UPDATE_RATE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // IOT Cadence Update Rate
        DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ID,
        DCI_IOT_CADENCE_UPDATE_RATE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_UNIT,
        nullptr,
        &DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_CADENCE_UPDATE_RATE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Group 0 Cadence
        DCI_GROUP0_CADENCE_REST_PARAM_ID,
        DCI_GROUP0_CADENCE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_GROUP0_CADENCE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_GROUP0_CADENCE_REST_PARAM_DESCRIPTION,
#endif
        DCI_GROUP0_CADENCE_REST_PARAM_UNIT,
        nullptr,
        &DCI_GROUP0_CADENCE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_GROUP0_CADENCE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_GROUP0_CADENCE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_GROUP0_CADENCE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Group 1 Cadence
        DCI_GROUP1_CADENCE_REST_PARAM_ID,
        DCI_GROUP1_CADENCE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_GROUP1_CADENCE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_GROUP1_CADENCE_REST_PARAM_DESCRIPTION,
#endif
        DCI_GROUP1_CADENCE_REST_PARAM_UNIT,
        nullptr,
        &DCI_GROUP1_CADENCE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_GROUP1_CADENCE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_GROUP1_CADENCE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_GROUP1_CADENCE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Group 2 Cadence
        DCI_GROUP2_CADENCE_REST_PARAM_ID,
        DCI_GROUP2_CADENCE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_GROUP2_CADENCE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_GROUP2_CADENCE_REST_PARAM_DESCRIPTION,
#endif
        DCI_GROUP2_CADENCE_REST_PARAM_UNIT,
        nullptr,
        &DCI_GROUP2_CADENCE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_GROUP2_CADENCE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_GROUP2_CADENCE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_GROUP2_CADENCE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Group 3 Cadence
        DCI_GROUP3_CADENCE_REST_PARAM_ID,
        DCI_GROUP3_CADENCE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_GROUP3_CADENCE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_GROUP3_CADENCE_REST_PARAM_DESCRIPTION,
#endif
        DCI_GROUP3_CADENCE_REST_PARAM_UNIT,
        nullptr,
        &DCI_GROUP3_CADENCE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_GROUP3_CADENCE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_GROUP3_CADENCE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_GROUP3_CADENCE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Logging Test 1
        DCI_LOG_TEST_1_REST_PARAM_ID,
        DCI_LOG_TEST_1_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOG_TEST_1_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOG_TEST_1_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOG_TEST_1_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOG_TEST_1_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOG_TEST_1_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOG_TEST_1_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Reset Cause
        DCI_RESET_CAUSE_REST_PARAM_ID,
        DCI_RESET_CAUSE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_RESET_CAUSE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_RESET_CAUSE_REST_PARAM_DESCRIPTION,
#endif
        DCI_RESET_CAUSE_REST_PARAM_UNIT,
        &DCI_RESET_CAUSE_REST_PARAM_BITFIELD_DESC_BLOCK,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_RESET_CAUSE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_RESET_CAUSE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_RESET_CAUSE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Logging event codes
        DCI_LOGGING_EVENT_CODE_REST_PARAM_ID,
        DCI_LOGGING_EVENT_CODE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOGGING_EVENT_CODE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOGGING_EVENT_CODE_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOGGING_EVENT_CODE_REST_PARAM_UNIT,
        nullptr,
        &DCI_LOGGING_EVENT_CODE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOGGING_EVENT_CODE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOGGING_EVENT_CODE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOGGING_EVENT_CODE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Device state
        DCI_DEVICE_STATE_REST_PARAM_ID,
        DCI_DEVICE_STATE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_DEVICE_STATE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_DEVICE_STATE_REST_PARAM_DESCRIPTION,
#endif
        DCI_DEVICE_STATE_REST_PARAM_UNIT,
        nullptr,
        &DCI_DEVICE_STATE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_DEVICE_STATE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_DEVICE_STATE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_DEVICE_STATE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Log Event 101 Test 
        DCI_LOG_EVENT_TEST_101_REST_PARAM_ID,
        DCI_LOG_EVENT_TEST_101_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOG_EVENT_TEST_101_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOG_EVENT_TEST_101_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOG_EVENT_TEST_101_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOG_EVENT_TEST_101_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOG_EVENT_TEST_101_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOG_EVENT_TEST_101_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Log Event 102 Test 
        DCI_LOG_EVENT_TEST_102_REST_PARAM_ID,
        DCI_LOG_EVENT_TEST_102_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOG_EVENT_TEST_102_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOG_EVENT_TEST_102_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOG_EVENT_TEST_102_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOG_EVENT_TEST_102_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOG_EVENT_TEST_102_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOG_EVENT_TEST_102_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // NV mem id
        DCI_NV_MEM_ID_REST_PARAM_ID,
        DCI_NV_MEM_ID_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_NV_MEM_ID_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_NV_MEM_ID_REST_PARAM_DESCRIPTION,
#endif
        DCI_NV_MEM_ID_REST_PARAM_UNIT,
        nullptr,
        &DCI_NV_MEM_ID_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_NV_MEM_ID_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_NV_MEM_ID_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_NV_MEM_ID_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // NV parameter
        DCI_NV_PARAMETER_ADDRESS_REST_PARAM_ID,
        DCI_NV_PARAMETER_ADDRESS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_NV_PARAMETER_ADDRESS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_NV_PARAMETER_ADDRESS_REST_PARAM_DESCRIPTION,
#endif
        DCI_NV_PARAMETER_ADDRESS_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_NV_PARAMETER_ADDRESS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_NV_PARAMETER_ADDRESS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_NV_PARAMETER_ADDRESS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // nv failed operation
        DCI_NV_FAIL_OPERATION_REST_PARAM_ID,
        DCI_NV_FAIL_OPERATION_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_NV_FAIL_OPERATION_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_NV_FAIL_OPERATION_REST_PARAM_DESCRIPTION,
#endif
        DCI_NV_FAIL_OPERATION_REST_PARAM_UNIT,
        nullptr,
        &DCI_NV_FAIL_OPERATION_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_NV_FAIL_OPERATION_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_NV_FAIL_OPERATION_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_NV_FAIL_OPERATION_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // PROC ID and IMAGE ID
        DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ID,
        DCI_LOG_PROC_IMAGE_ID_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOG_PROC_IMAGE_ID_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOG_PROC_IMAGE_ID_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOG_PROC_IMAGE_ID_REST_PARAM_UNIT,
        nullptr,
        &DCI_LOG_PROC_IMAGE_ID_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOG_PROC_IMAGE_ID_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOG_PROC_IMAGE_ID_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOG_PROC_IMAGE_ID_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Firmware revision for logging
        DCI_LOG_FIRMWARE_VERSION_REST_PARAM_ID,
        DCI_LOG_FIRMWARE_VERSION_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOG_FIRMWARE_VERSION_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOG_FIRMWARE_VERSION_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOG_FIRMWARE_VERSION_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOG_FIRMWARE_VERSION_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOG_FIRMWARE_VERSION_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOG_FIRMWARE_VERSION_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Logged user name
        DCI_LOGGED_USERNAME_REST_PARAM_ID,
        DCI_LOGGED_USERNAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOGGED_USERNAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOGGED_USERNAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOGGED_USERNAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOGGED_USERNAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOGGED_USERNAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOGGED_USERNAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Log IP address
        DCI_LOG_IP_ADDRESS_REST_PARAM_ID,
        DCI_LOG_IP_ADDRESS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOG_IP_ADDRESS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOG_IP_ADDRESS_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOG_IP_ADDRESS_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_LOG_IP_ADDRESS_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOG_IP_ADDRESS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOG_IP_ADDRESS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOG_IP_ADDRESS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Log port number
        DCI_LOG_PORT_NUMBER_REST_PARAM_ID,
        DCI_LOG_PORT_NUMBER_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LOG_PORT_NUMBER_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LOG_PORT_NUMBER_REST_PARAM_DESCRIPTION,
#endif
        DCI_LOG_PORT_NUMBER_REST_PARAM_UNIT,
        nullptr,
        &DCI_LOG_PORT_NUMBER_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LOG_PORT_NUMBER_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LOG_PORT_NUMBER_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LOG_PORT_NUMBER_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Modbus TCP enable or disable
        DCI_MODBUS_TCP_ENABLE_REST_PARAM_ID,
        DCI_MODBUS_TCP_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MODBUS_TCP_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MODBUS_TCP_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_MODBUS_TCP_ENABLE_REST_PARAM_UNIT,
        nullptr,
        &DCI_MODBUS_TCP_ENABLE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MODBUS_TCP_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MODBUS_TCP_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MODBUS_TCP_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Bacnet IP enable or disable
        DCI_BACNET_IP_ENABLE_REST_PARAM_ID,
        DCI_BACNET_IP_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_BACNET_IP_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_BACNET_IP_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_BACNET_IP_ENABLE_REST_PARAM_UNIT,
        nullptr,
        &DCI_BACNET_IP_ENABLE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_BACNET_IP_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_BACNET_IP_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_BACNET_IP_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Enable Session Takeover
        DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_ID,
        DCI_ENABLE_SESSION_TAKEOVER_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_DESCRIPTION,
#endif
        DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ENABLE_SESSION_TAKEOVER_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Maximum Concurrent Sessions
        DCI_MAX_CONCURRENT_SESSION_REST_PARAM_ID,
        DCI_MAX_CONCURRENT_SESSION_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MAX_CONCURRENT_SESSION_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MAX_CONCURRENT_SESSION_REST_PARAM_DESCRIPTION,
#endif
        DCI_MAX_CONCURRENT_SESSION_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MAX_CONCURRENT_SESSION_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MAX_CONCURRENT_SESSION_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MAX_CONCURRENT_SESSION_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // User Absolute Timeout
        DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_ID,
        DCI_ABSOLUTE_TIMEOUT_SEC_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_DESCRIPTION,
#endif
        DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_ABSOLUTE_TIMEOUT_SEC_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // User Lock Time
        DCI_USER_LOCK_TIME_SEC_REST_PARAM_ID,
        DCI_USER_LOCK_TIME_SEC_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_USER_LOCK_TIME_SEC_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_USER_LOCK_TIME_SEC_REST_PARAM_DESCRIPTION,
#endif
        DCI_USER_LOCK_TIME_SEC_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_USER_LOCK_TIME_SEC_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_USER_LOCK_TIME_SEC_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_USER_LOCK_TIME_SEC_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Maximum Failed Login Attempts
        DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_ID,
        DCI_MAX_FAILED_LOGIN_ATTEMPTS_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_DESCRIPTION,
#endif
        DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MAX_FAILED_LOGIN_ATTEMPTS_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // CORS Origin Type
        DCI_CORS_TYPE_REST_PARAM_ID,
        DCI_CORS_TYPE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_CORS_TYPE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_CORS_TYPE_REST_PARAM_DESCRIPTION,
#endif
        DCI_CORS_TYPE_REST_PARAM_UNIT,
        nullptr,
        &DCI_CORS_TYPE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_CORS_TYPE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_CORS_TYPE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_CORS_TYPE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Trusted IP filter for BACnet IP Enable
        DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_ID,
        DCI_TRUSTED_IP_BACNET_IP_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_TRUSTED_IP_BACNET_IP_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Trusted IP filter for EIP Enable
        DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_ID,
        DCI_TRUSTED_IP_EIP_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_TRUSTED_IP_EIP_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Device Certificate Generation Control
        DCI_DEVICE_CERT_CONTROL_REST_PARAM_ID,
        DCI_DEVICE_CERT_CONTROL_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_DEVICE_CERT_CONTROL_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_DEVICE_CERT_CONTROL_REST_PARAM_DESCRIPTION,
#endif
        DCI_DEVICE_CERT_CONTROL_REST_PARAM_UNIT,
        nullptr,
        &DCI_DEVICE_CERT_CONTROL_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_DEVICE_CERT_CONTROL_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_DEVICE_CERT_CONTROL_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_DEVICE_CERT_CONTROL_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // mDNS lwm2m Service Discovery Enable
        DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_ID,
        DCI_MDNS_LWM2M_SERVICE_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MDNS_LWM2M_SERVICE_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // mDNS Server 1 Hostname
        DCI_MDNS_SERVER1_NAME_REST_PARAM_ID,
        DCI_MDNS_SERVER1_NAME_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MDNS_SERVER1_NAME_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MDNS_SERVER1_NAME_REST_PARAM_DESCRIPTION,
#endif
        DCI_MDNS_SERVER1_NAME_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MDNS_SERVER1_NAME_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MDNS_SERVER1_NAME_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MDNS_SERVER1_NAME_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // mDNS Server 1 IP Address
        DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_ID,
        DCI_MDNS_SERVER1_IP_ADDR_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_DESCRIPTION,
#endif
        DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        &DCI_MDNS_SERVER1_IP_ADDR_FORMATTER,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MDNS_SERVER1_IP_ADDR_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // mDNS Server 1 Service Port
        DCI_MDNS_SERVER1_PORT_REST_PARAM_ID,
        DCI_MDNS_SERVER1_PORT_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_MDNS_SERVER1_PORT_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_MDNS_SERVER1_PORT_REST_PARAM_DESCRIPTION,
#endif
        DCI_MDNS_SERVER1_PORT_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_MDNS_SERVER1_PORT_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_MDNS_SERVER1_PORT_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_MDNS_SERVER1_PORT_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Open Source License
        DCI_LICENSE_INFORMATION_REST_PARAM_ID,
        DCI_LICENSE_INFORMATION_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LICENSE_INFORMATION_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LICENSE_INFORMATION_REST_PARAM_DESCRIPTION,
#endif
        DCI_LICENSE_INFORMATION_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LICENSE_INFORMATION_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LICENSE_INFORMATION_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LICENSE_INFORMATION_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // IOT Connection Status reason
        DCI_IOT_CONN_STAT_REASON_REST_PARAM_ID,
        DCI_IOT_CONN_STAT_REASON_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_IOT_CONN_STAT_REASON_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_IOT_CONN_STAT_REASON_REST_PARAM_DESCRIPTION,
#endif
        DCI_IOT_CONN_STAT_REASON_REST_PARAM_UNIT,
        nullptr,
        &DCI_IOT_CONN_STAT_REASON_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_IOT_CONN_STAT_REASON_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_IOT_CONN_STAT_REASON_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_IOT_CONN_STAT_REASON_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Preferred Language Setting
        DCI_LANG_PREF_SETTING_REST_PARAM_ID,
        DCI_LANG_PREF_SETTING_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_LANG_PREF_SETTING_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_LANG_PREF_SETTING_REST_PARAM_DESCRIPTION,
#endif
        DCI_LANG_PREF_SETTING_REST_PARAM_UNIT,
        nullptr,
        &DCI_LANG_PREF_SETTING_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_LANG_PREF_SETTING_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_LANG_PREF_SETTING_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_LANG_PREF_SETTING_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Common Language Preferrence
        DCI_COMMON_LANG_PREF_REST_PARAM_ID,
        DCI_COMMON_LANG_PREF_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_COMMON_LANG_PREF_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_COMMON_LANG_PREF_REST_PARAM_DESCRIPTION,
#endif
        DCI_COMMON_LANG_PREF_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_COMMON_LANG_PREF_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_COMMON_LANG_PREF_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_COMMON_LANG_PREF_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Data Log
        DCI_DATA_LOG_REST_PARAM_ID,
        DCI_DATA_LOG_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_DATA_LOG_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_DATA_LOG_REST_PARAM_DESCRIPTION,
#endif
        DCI_DATA_LOG_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_DATA_LOG_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_DATA_LOG_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_DATA_LOG_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Event Log
        DCI_EVENT_LOG_REST_PARAM_ID,
        DCI_EVENT_LOG_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_EVENT_LOG_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_EVENT_LOG_REST_PARAM_DESCRIPTION,
#endif
        DCI_EVENT_LOG_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_EVENT_LOG_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_EVENT_LOG_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_EVENT_LOG_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Audit-Power Log
        DCI_AUDIT_POWER_LOG_REST_PARAM_ID,
        DCI_AUDIT_POWER_LOG_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_AUDIT_POWER_LOG_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_AUDIT_POWER_LOG_REST_PARAM_DESCRIPTION,
#endif
        DCI_AUDIT_POWER_LOG_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_AUDIT_POWER_LOG_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_AUDIT_POWER_LOG_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_AUDIT_POWER_LOG_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Audit-Fw-Update Log
        DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_ID,
        DCI_AUDIT_FW_UPGRADE_LOG_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_DESCRIPTION,
#endif
        DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_AUDIT_FW_UPGRADE_LOG_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Audit-User Log
        DCI_AUDIT_USER_LOG_REST_PARAM_ID,
        DCI_AUDIT_USER_LOG_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_AUDIT_USER_LOG_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_AUDIT_USER_LOG_REST_PARAM_DESCRIPTION,
#endif
        DCI_AUDIT_USER_LOG_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_AUDIT_USER_LOG_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_AUDIT_USER_LOG_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_AUDIT_USER_LOG_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Audit-Config Log
        DCI_AUDIT_CONFIG_LOG_REST_PARAM_ID,
        DCI_AUDIT_CONFIG_LOG_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_AUDIT_CONFIG_LOG_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_AUDIT_CONFIG_LOG_REST_PARAM_DESCRIPTION,
#endif
        DCI_AUDIT_CONFIG_LOG_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_AUDIT_CONFIG_LOG_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_AUDIT_CONFIG_LOG_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_AUDIT_CONFIG_LOG_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // Device Up Time
        DCI_DEVICE_UP_TIME_SEC_REST_PARAM_ID,
        DCI_DEVICE_UP_TIME_SEC_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_DEVICE_UP_TIME_SEC_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_DEVICE_UP_TIME_SEC_REST_PARAM_DESCRIPTION,
#endif
        DCI_DEVICE_UP_TIME_SEC_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_DEVICE_UP_TIME_SEC_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_DEVICE_UP_TIME_SEC_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_DEVICE_UP_TIME_SEC_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // PTP enable or disable
        DCI_PTP_ENABLE_REST_PARAM_ID,
        DCI_PTP_ENABLE_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_PTP_ENABLE_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_PTP_ENABLE_REST_PARAM_DESCRIPTION,
#endif
        DCI_PTP_ENABLE_REST_PARAM_UNIT,
        nullptr,
        &DCI_PTP_ENABLE_REST_PARAM_ENUM_DESC_BLOCK,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_PTP_ENABLE_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_PTP_ENABLE_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_PTP_ENABLE_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
    {
        // SNTP timeoffset max in microseconds
        DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_ID,
        DCI_SNTP_TIME_OFFSET_DEBUG_DCID,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_NAME,
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_DESCRIPTION,
#endif
        DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_UNIT,
        nullptr,
        nullptr,
        nullptr,
#ifndef REMOVE_REST_PARAM_NAME_TEXT
        (static_cast<uint8_t>(sizeof(DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_NAME))-1U),
#endif
        (static_cast<uint8_t>(sizeof(DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_UNIT)) -1U),
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
        (static_cast<uint16_t>(sizeof(DCI_SNTP_TIME_OFFSET_DEBUG_REST_PARAM_DESCRIPTION)) -1U),
#endif
        7U,
        99U
    },
};


//*******************************************************
//******     The Predefined Assemblies 
//*******************************************************

static const char_t REST_DCI_DATA_PREDEFINED_ASSY_1_NAME []= "Assembly 1";
static const REST_PARAM_ID_TD REST_DCI_DATA_PREDEFINED_ASSY_1_PARAM_LIST[] =
{
    DCI_PRODUCT_SERIAL_NUM_REST_PARAM_ID,
    DCI_VENDOR_NAME_REST_PARAM_ID,
    DCI_PRODUCT_CODE_REST_PARAM_ID,
    DCI_PRODUCT_CODE_ASCII_REST_PARAM_ID,
    DCI_VENDOR_URL_REST_PARAM_ID,
    DCI_PRODUCT_NAME_REST_PARAM_ID,
    DCI_MODEL_NAME_REST_PARAM_ID,
    DCI_USER_APP_NAME_REST_PARAM_ID,
};
static const char_t REST_DCI_DATA_PREDEFINED_ASSY_12_NAME []= "Network Configuration";
static const REST_PARAM_ID_TD REST_DCI_DATA_PREDEFINED_ASSY_12_PARAM_LIST[] =
{
    DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_REST_PARAM_ID,
    DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_REST_PARAM_ID,
    DCI_IP_ADDRESS_ALLOCATION_METHOD_REST_PARAM_ID,
    DCI_MODBUS_TCP_COMM_TIMEOUT_REST_PARAM_ID,
    DCI_ETHERNET_MAC_ADDRESS_REST_PARAM_ID,
    DCI_ACTIVE_IP_ADDRESS_REST_PARAM_ID,
    DCI_ACTIVE_SUBNET_MASK_REST_PARAM_ID,
    DCI_ACTIVE_DEFAULT_GATEWAY_REST_PARAM_ID,
    DCI_STATIC_IP_ADDRESS_REST_PARAM_ID,
    DCI_STATIC_SUBNET_MASK_REST_PARAM_ID,
    DCI_STATIC_DEFAULT_GATEWAY_REST_PARAM_ID,
};
static const char_t REST_DCI_DATA_PREDEFINED_ASSY_13_NAME []= "RTC Time";
static const REST_PARAM_ID_TD REST_DCI_DATA_PREDEFINED_ASSY_13_PARAM_LIST[] =
{
    DCI_UNIX_EPOCH_TIME_REST_PARAM_ID,
    DCI_TIME_OFFSET_REST_PARAM_ID,
    DCI_DATE_FORMAT_REST_PARAM_ID,
    DCI_TIME_FORMAT_REST_PARAM_ID,
};

static const REST_PREDEFINED_ASSY_TD REST_DCI_DATA_PREDEFINED_ASSY_LIST[] =
{
    {
        1U,
        static_cast<uint16_t>(sizeof(REST_DCI_DATA_PREDEFINED_ASSY_1_PARAM_LIST)/sizeof(REST_PARAM_ID_TD)),
        REST_DCI_DATA_PREDEFINED_ASSY_1_NAME,
        (static_cast<uint8_t>(sizeof(REST_DCI_DATA_PREDEFINED_ASSY_1_NAME)) -1U),
        REST_DCI_DATA_PREDEFINED_ASSY_1_PARAM_LIST,
        7U,
        99U
    },
    {
        12U,
        static_cast<uint16_t>(sizeof(REST_DCI_DATA_PREDEFINED_ASSY_12_PARAM_LIST)/sizeof(REST_PARAM_ID_TD)),
        REST_DCI_DATA_PREDEFINED_ASSY_12_NAME,
        (static_cast<uint8_t>(sizeof(REST_DCI_DATA_PREDEFINED_ASSY_12_NAME)) -1U),
        REST_DCI_DATA_PREDEFINED_ASSY_12_PARAM_LIST,
        7U,
        99U
    },
    {
        13U,
        static_cast<uint16_t>(sizeof(REST_DCI_DATA_PREDEFINED_ASSY_13_PARAM_LIST)/sizeof(REST_PARAM_ID_TD)),
        REST_DCI_DATA_PREDEFINED_ASSY_13_NAME,
        (static_cast<uint8_t>(sizeof(REST_DCI_DATA_PREDEFINED_ASSY_13_NAME)) -1U),
        REST_DCI_DATA_PREDEFINED_ASSY_13_PARAM_LIST,
        7U,
        99U
    },
};


//*******************************************************
//******     The Custom Assemblies 
//*******************************************************


static const rest_pid16_t REST_DCI_DATA_NUM_OF_CUSTOM_ASSY = TOTAL_NUM_OF_CUSTOM_ASSEMBLIES;
static const rest_pid8_t REST_DCI_DATA_NUM_OF_PARAM_IN_A_CUSTOM_ASSY = MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY;

static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_1[] = "USER ASM1";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_2[] = "USER ASM2";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_3[] = "USER ASM3";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_4[] = "USER ASM4";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_5[] = "USER ASM5";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_6[] = "USER ASM6";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_7[] = "USER ASM7";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_8[] = "USER ASM8";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_9[] = "USER ASM9";
static const char_t REST_DCI_DATA_CUSTOM_ASSY_NAME_10[] = "USER ASM10";

static const REST_CUSTOM_ASSY_TD REST_DCI_DATA_CUSTOM_ASSY_LIST[] = {
    {
        101U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_1,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_1) )-1U),
        DCI_VALID_BITS_ASSY_0_DCID,
        DCI_MEMBERS_ASSY_0_DCID,
        7U,
        99U
    },
    {
        102U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_2,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_2) )-1U),
        DCI_VALID_BITS_ASSY_1_DCID,
        DCI_MEMBERS_ASSY_1_DCID,
        7U,
        99U
    },
    {
        103U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_3,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_3) )-1U),
        DCI_VALID_BITS_ASSY_2_DCID,
        DCI_MEMBERS_ASSY_2_DCID,
        7U,
        99U
    },
    {
        104U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_4,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_4) )-1U),
        DCI_VALID_BITS_ASSY_3_DCID,
        DCI_MEMBERS_ASSY_3_DCID,
        7U,
        99U
    },
    {
        105U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_5,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_5) )-1U),
        DCI_VALID_BITS_ASSY_4_DCID,
        DCI_MEMBERS_ASSY_4_DCID,
        7U,
        99U
    },
    {
        106U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_6,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_6) )-1U),
        DCI_VALID_BITS_ASSY_5_DCID,
        DCI_MEMBERS_ASSY_5_DCID,
        7U,
        99U
    },
    {
        107U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_7,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_7) )-1U),
        DCI_VALID_BITS_ASSY_6_DCID,
        DCI_MEMBERS_ASSY_6_DCID,
        7U,
        99U
    },
    {
        108U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_8,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_8) )-1U),
        DCI_VALID_BITS_ASSY_7_DCID,
        DCI_MEMBERS_ASSY_7_DCID,
        7U,
        99U
    },
    {
        109U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_9,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_9) )-1U),
        DCI_VALID_BITS_ASSY_8_DCID,
        DCI_MEMBERS_ASSY_8_DCID,
        7U,
        99U
    },
    {
        110U,
        REST_DCI_DATA_CUSTOM_ASSY_NAME_10,
        (static_cast<uint8_t>( sizeof(REST_DCI_DATA_CUSTOM_ASSY_NAME_10) )-1U),
        DCI_VALID_BITS_ASSY_9_DCID,
        DCI_MEMBERS_ASSY_9_DCID,
        7U,
        99U
    },
};

//*******************************************************
//******     The Target Definition Structure.
//*******************************************************

const REST_TARGET_INFO_ST_TD rest_dci_data_target_info = 
{
    NUM_TOTAL_REST_DCI_DATA_PARAMS,
    base_dci_rest_data_param_list,
    {
        REST_DCI_DATA_ALL_PARAM_ACCESS_R,
        REST_DCI_DATA_ALL_PARAM_ACCESS_W,
    },
    static_cast<uint16_t>(sizeof(REST_DCI_DATA_PREDEFINED_ASSY_LIST) / sizeof(REST_PREDEFINED_ASSY_TD)),
    REST_DCI_DATA_PREDEFINED_ASSY_LIST,
    REST_DCI_DATA_NUM_OF_CUSTOM_ASSY,
    REST_DCI_DATA_NUM_OF_PARAM_IN_A_CUSTOM_ASSY,
    REST_DCI_DATA_CUSTOM_ASSY_LIST

};


//*******************************************************
//******     The User Management 
//*******************************************************

const AUTH_STRUCT_TD auth_struct[] = {
    {
        "Admin",
        99,
        "This is an Administrator having all privileges",
    },
    {
        "Engineer",
        31,
        "This is an Engineer Role having Product configuration Privileges",
    },
    {
        "Operator",
        15,
        "This is an Operator Role having Product control privileges",
    },
    {
        "Viewer",
        7,
        "This is a Role having readonly access",
    },
};

const uint8_t NUM_OF_AUTH_LEVELS = sizeof (auth_struct) / sizeof(AUTH_STRUCT_TD);

const DCI_USER_MGMT_DB_TD USER_MGMT_USER_LIST[] = {
    {
        DCI_USER_1_FULLNAME_DCID,
        DCI_USER_1_USERNAME_DCID,
        DCI_USER_1_PWD_DCID,
        DCI_USER_1_ROLE_DCID,
        DCI_USER_1_PWD_SET_EPOCH_TIME_DCID,
        DCI_USER_1_LAST_LOGIN_EPOCH_TIME_DCID,
        DCI_USER_1_PWD_COMPLEXITY_DCID,
        DCI_USER_1_PWD_AGING_DCID,
    },
    {
        DCI_USER_2_FULLNAME_DCID,
        DCI_USER_2_USERNAME_DCID,
        DCI_USER_2_PWD_DCID,
        DCI_USER_2_ROLE_DCID,
        DCI_USER_2_PWD_SET_EPOCH_TIME_DCID,
        DCI_USER_2_LAST_LOGIN_EPOCH_TIME_DCID,
        DCI_USER_2_PWD_COMPLEXITY_DCID,
        DCI_USER_2_PWD_AGING_DCID,
    },
    {
        DCI_USER_3_FULLNAME_DCID,
        DCI_USER_3_USERNAME_DCID,
        DCI_USER_3_PWD_DCID,
        DCI_USER_3_ROLE_DCID,
        DCI_USER_3_PWD_SET_EPOCH_TIME_DCID,
        DCI_USER_3_LAST_LOGIN_EPOCH_TIME_DCID,
        DCI_USER_3_PWD_COMPLEXITY_DCID,
        DCI_USER_3_PWD_AGING_DCID,
    },
    {
        DCI_USER_4_FULLNAME_DCID,
        DCI_USER_4_USERNAME_DCID,
        DCI_USER_4_PWD_DCID,
        DCI_USER_4_ROLE_DCID,
        DCI_USER_4_PWD_SET_EPOCH_TIME_DCID,
        DCI_USER_4_LAST_LOGIN_EPOCH_TIME_DCID,
        DCI_USER_4_PWD_COMPLEXITY_DCID,
        DCI_USER_4_PWD_AGING_DCID,
    },
    {
        DCI_USER_5_FULLNAME_DCID,
        DCI_USER_5_USERNAME_DCID,
        DCI_USER_5_PWD_DCID,
        DCI_USER_5_ROLE_DCID,
        DCI_USER_5_PWD_SET_EPOCH_TIME_DCID,
        DCI_USER_5_LAST_LOGIN_EPOCH_TIME_DCID,
        DCI_USER_5_PWD_COMPLEXITY_DCID,
        DCI_USER_5_PWD_AGING_DCID,
    },
    {
        DCI_USER_6_FULLNAME_DCID,
        DCI_USER_6_USERNAME_DCID,
        DCI_USER_6_PWD_DCID,
        DCI_USER_6_ROLE_DCID,
        DCI_USER_6_PWD_SET_EPOCH_TIME_DCID,
        DCI_USER_6_LAST_LOGIN_EPOCH_TIME_DCID,
        DCI_USER_6_PWD_COMPLEXITY_DCID,
        DCI_USER_6_PWD_AGING_DCID,
    },
};

const uint8_t MAX_NUM_OF_AUTH_USERS = sizeof (USER_MGMT_USER_LIST) / sizeof(DCI_USER_MGMT_DB_TD);

