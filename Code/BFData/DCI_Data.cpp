/*
*****************************************************************************************
*   @file
*   @details See header file for module overview.
*   @copyright 2024 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#include "Includes.h"
#include "DCI_Data.h"
#include "NV_Address.h"
#include "DCI_Constants.h"


//*******************************************************
//******     Static Variables
//*******************************************************

static const bool True = TRUE;
static const bool False = FALSE;


//*******************************************************
//******     The Ram Value
//*******************************************************

static float64_t DCI_BLE_TEST_READING_3_RAM = 558;
static float64_t DCI_IOT_ActiveEnergyL1_RAM = 0;
static float64_t DCI_IOT_ActiveEnergyL2_RAM = 0;
static float64_t DCI_IOT_ReactiveEnergyL1_RAM = 0;
static float64_t DCI_IOT_ReactiveEnergyL2_RAM = 0;
static float64_t DCI_IOT_ApparentEnergyL1_RAM = 0;
static float64_t DCI_IOT_ApparentEnergyL2_RAM = 0;
static float64_t DCI_IOT_ReverseActiveEnergyL1_RAM = 0;
static float64_t DCI_IOT_ReverseReactiveEnergyL1_RAM = 0;
static float64_t DCI_IOT_ReverseApparentEnergyL1_RAM = 0;
static float64_t DCI_IOT_ReverseActiveEnergyL2_RAM = 0;
static float64_t DCI_IOT_ReverseReactiveEnergyL2_RAM = 0;
static float64_t DCI_IOT_ReverseApparentEnergyL2_RAM = 0;
static uint64_t DCI_BLE_TEST_SETTING_C_RAM = 54321456UL;
static uint64_t DCI_IEC61850_IND1_T_RAM;
static uint64_t DCI_IEC61850_IND2_T_RAM;
static uint64_t DCI_IEC61850_IND3_T_RAM;
static uint64_t DCI_IEC61850_IND4_T_RAM;
static uint64_t DCI_IEC61850_ALM1_T_RAM;
static uint64_t DCI_IEC61850_ALM2_T_RAM;
static uint64_t DCI_IEC61850_ALM5_T_RAM;
static uint64_t DCI_IEC61850_ALM6_T_RAM;
static uint64_t DCI_IOT_FirmwareVersion_RAM;
static int64_t DCI_BLE_TEST_EVENT_LIST_RAM = -123;
static int64_t DCI_SNTP_TIME_OFFSET_DEBUG_RAM = 0;
static float32_t DCI_LOG_TEST_1_RAM;
static float32_t DCI_BLE_TEST_READING_2_RAM = 500.0F;
static float32_t DCI_IOT_VoltageL1_RAM = 0.0F;
static float32_t DCI_IOT_VoltageL2_RAM = 0.0F;
static float32_t DCI_IOT_CurrentL1_RAM = 0.0F;
static float32_t DCI_IOT_CurrentL2_RAM = 0.0F;
static float32_t DCI_IOT_ActivePowerL1_RAM = 0.0F;
static float32_t DCI_IOT_ActivePowerL2_RAM = 0.0F;
static float32_t DCI_IOT_ReactivePowerL1_RAM = 0.0F;
static float32_t DCI_IOT_ReactivePowerL2_RAM = 0.0F;
static float32_t DCI_IOT_ApparentPowerL1_RAM = 0.0F;
static float32_t DCI_IOT_ApparentPowerL2_RAM = 0.0F;
static float32_t DCI_IOT_PowerFactorL1_RAM = 0.0F;
static float32_t DCI_IOT_PowerFactorL2_RAM = 0.0F;
static float32_t DCI_IOT_FrequencyL1_RAM = 0.0F;
static float32_t DCI_IOT_FrequencyL2_RAM = 0.0F;
static float32_t DCI_IOT_Load_Percentage_RAM = 0.0F;
static float32_t DCI_IOT_TEMPERATURE_RAM = 0.0F;
static uint32_t DCI_PRODUCT_SERIAL_NUM_RAM;
static uint32_t DCI_APP_FIRM_VER_NUM_RAM = CONST_FIRMWARE_VER_NUM;
static uint32_t DCI_APP_FIRM_UPGRADE_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_WEB_FIRM_VER_NUM_RAM;
static uint32_t DCI_MULTICAST_ENABLE_RAM = 0x00000010U;
static uint32_t DCI_BROADCAST_ENABLE_RAM = 0x00000000U;
static uint32_t DCI_EXAMPLE_MODBUS_SERIAL_BAUD_RAM = 19200U;
static uint32_t DCI_RTC_TIME_RAM = 0x0034220AU;
static uint32_t DCI_RTC_DATE_RAM = 0x07E00319U;
static uint32_t DCI_PRODUCT_PROC_SERIAL_NUM_RAM;
static uint32_t DCI_USER_1_PWD_SET_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_1_LAST_LOGIN_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_2_PWD_SET_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_2_LAST_LOGIN_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_3_PWD_SET_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_3_LAST_LOGIN_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_4_PWD_SET_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_4_LAST_LOGIN_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_5_PWD_SET_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_5_LAST_LOGIN_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_6_PWD_SET_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_USER_6_LAST_LOGIN_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_IP_DWORD_NOT_CONNECTED_RAM;
static uint32_t DCI_OP_DWORD_NOT_CONNECTED_RAM;
static uint32_t DCI_SAMPLE_APP_INPUT_DWORD_RAM;
static uint32_t DCI_SAMPLE_APP_OUTPUT_DWORD_RAM = 1U;
static uint32_t DCI_SAMPLE_ACYCLIC_PARAMETER_1_RAM = 0x11223344U;
static uint32_t DCI_SAMPLE_ACYCLIC_PARAMETER_5_RAM = 0x05500550U;
static uint32_t DCI_ETH_NAME_SERVER_1_RAM = 0U;
static uint32_t DCI_ETH_NAME_SERVER_2_RAM = 0U;
static uint32_t DCI_DYN_IN_ASM_MEMBER_1_RAM = 0x00000001U;
static uint32_t DCI_DYN_IN_ASM_MEMBER_2_RAM = 0x00000001U;
static uint32_t DCI_DYN_IN_ASM_MEMBER_3_RAM = 0x00000001U;
static uint32_t DCI_DYN_IN_ASM_MEMBER_4_RAM = 0x00000001U;
static uint32_t DCI_DYN_IN_ASM_MEMBER_5_RAM = 0x00000001U;
static uint32_t DCI_DYN_IN_ASM_MEMBER_6_RAM = 0x00000001U;
static uint32_t DCI_DYN_IN_ASM_MEMBER_7_RAM = 0x00000001U;
static uint32_t DCI_DYN_IN_ASM_MEMBER_8_RAM = 0x00000001U;
static uint32_t DCI_DYN_OUT_ASM_MEMBER_1_RAM = 0x00000001U;
static uint32_t DCI_DYN_OUT_ASM_MEMBER_2_RAM = 0x00000001U;
static uint32_t DCI_DYN_OUT_ASM_MEMBER_3_RAM = 0x00000001U;
static uint32_t DCI_DYN_OUT_ASM_MEMBER_4_RAM = 0x00000001U;
static uint32_t DCI_BACNET_IP_INSTANCE_NUM_RAM = 0U;
static uint32_t DCI_BACNET_MSTP_INSTANCE_NUM_RAM = 0U;
static uint32_t DCI_BACNET_MSTP_BAUD_RAM = 38400U;
static uint32_t DCI_BACNET_DATABASE_REVISION_RAM = 1U;
static uint32_t DCI_AV_ANY_PARAM_VAL_RAM = 0U;
static uint32_t DCI_UNIX_EPOCH_TIME_RAM = 946684800U;
static uint32_t DCI_UNIX_EPOCH_TIME_64_BIT_RAM[2] = 
        { 946684800U, 0U };
static uint32_t DCI_FAULT_CATCHER_RAM[22];
static uint32_t DCI_SNTP_UPDATE_TIME_RAM = 3600000U;
static uint32_t DCI_LOG_INTERVAL_TIME_MS_RAM = 0U;
static uint32_t DCI_BLE_TEST_SETTING_B_RAM = 0x34566787U;
static uint32_t DCI_BLE_TEST_SETTING_7_RAM = 2000U;
static uint32_t DCI_BLE_TEST_DEVICE_TIME_RAM = 1514955660U;
static uint32_t DCI_CR_PRIORITY_TIMING_RAM[6] = 
        { 0U, 0U, 0U, 0U, 0U, 0U };
static uint32_t DCI_OS_PRIORITY_TIMING_RAM[8] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint32_t DCI_NV_PARAMETER_ADDRESS_RAM;
static uint32_t DCI_RESET_LINE_NUMBER_RAM = 0U;
static uint32_t DCI_RESET_OS_TASK_PRIORITY_RAM = 0U;
static uint32_t DCI_RESET_OS_STACK_SIZE_RAM = 0U;
static uint32_t DCI_RESET_CR_TASK_RAM = 0U;
static uint32_t DCI_RESET_CR_TASK_PARAM_RAM = 0U;
static uint32_t DCI_RESET_CSTACK_SIZE_RAM = 0U;
static uint32_t DCI_RESET_HIGHEST_OS_STACK_SIZE_RAM = 0U;
static uint32_t DCI_RESET_HEAP_USAGE_PERCENT_RAM = 0U;
static uint32_t DCI_RESET_SOURCE_RAM = 0U;
static uint32_t DCI_ABSOLUTE_TIMEOUT_SEC_RAM = 21600U;
static uint32_t DCI_J1939_MSG_EX9_RAM = 0U;
static uint32_t DCI_J1939_MSG_EX10_RAM = 0U;
static uint32_t DCI_LANG_FIRM_VER_NUM_RAM;
static uint32_t DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_RAM = 0U;
static uint32_t DCI_BLE_LOG_INDEX_RAM = 0U;
static uint32_t DCI_DEVICE_UP_TIME_SEC_RAM = 0U;
static uint32_t DCI_PTP_SECONDS_TIME_RAM = 0U;
static uint32_t DCI_PTP_NANOSECONDS_TIME_RAM = 0U;
static uint32_t DCI_IEC61850_CONFIG_REV1_RAM = 0U;
static uint32_t DCI_IEC61850_CONFIG_REV2_RAM = 0U;
static uint32_t DCI_IEC61850_CONFIG_REV4_RAM = 0U;
static uint32_t DCI_IEC61850_CONFIG_REV5_RAM = 0U;
static uint32_t DCI_IEC61850_SAV_CONFIG_REV1_RAM = 1U;
static uint32_t DCI_IEC61850_SAV_PHA_AMP1_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHB_AMP1_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHC_AMP1_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHN_AMP1_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHA_VOLT1_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHB_VOLT1_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHC_VOLT1_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHN_VOLT1_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHA_AMP2_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHB_AMP2_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHC_AMP2_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHN_AMP2_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHA_VOLT2_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHB_VOLT2_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHC_VOLT2_RAM[8];
static uint32_t DCI_IEC61850_SAV_PHN_VOLT2_RAM[8];
static uint32_t DCI_IOT_ResetBreaker_RAM;
static uint32_t DCI_IOT_POWER_UP_STATE_RAM = 1U;
static int32_t DCI_GROUP0_CADENCE_RAM = 10000;
static int32_t DCI_GROUP1_CADENCE_RAM = 60000;
static int32_t DCI_GROUP2_CADENCE_RAM = 1000;
static int32_t DCI_GROUP3_CADENCE_RAM = 1000;
static int32_t DCI_BLE_DEVICE_PASSKEY_RAM = 123456;
static int32_t DCI_BLE_TEST_SETTING_6_RAM = -600;
static int32_t DCI_CLIENT_AP_SECURITY_RAM = 6;
static uint16_t DCI_FIRMWARE_CRC_VAL_RAM;
static uint16_t DCI_RESET_COUNTER_RAM = 0U;
static uint16_t DCI_IDLE_LOOP_TIME_AVE_RAM;
static uint16_t DCI_IDLE_LOOP_TIME_MAX_RAM;
static uint16_t DCI_IDLE_LOOP_TIME_MIN_RAM;
static uint16_t DCI_PWR_BRD_DIP_SWITCHES_RAM = 0U;
static uint16_t DCI_PHY1_LINK_SPEED_SELECT_RAM = 100U;
static uint16_t DCI_PHY1_LINK_SPEED_ACTUAL_RAM;
static uint16_t DCI_PHY2_LINK_SPEED_SELECT_RAM = 100U;
static uint16_t DCI_PHY2_LINK_SPEED_ACTUAL_RAM;
static uint16_t DCI_MODBUS_TCP_COMM_TIMEOUT_RAM = 0U;
static uint16_t DCI_MODBUS_SERIAL_COMM_TIMEOUT_RAM = 0U;
static uint16_t DCI_SIMPLE_APP_CONTROL_WORD_RAM = 0U;
static uint16_t DCI_SIMPLE_APP_STATUS_WORD_RAM = 0U;
static uint16_t DCI_MEMBERS_ASSY_0_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_1_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_2_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_3_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_4_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_5_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_6_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_7_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_8_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_MEMBERS_ASSY_9_RAM[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint16_t DCI_EXAMPLE_PWM_FREQUENCY_RAM = 1000U;
static uint16_t DCI_EXAMPLE_MODBUS_SERIAL_PARITY_RAM = 0U;
static uint16_t DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_RAM = 0U;
static uint16_t DCI_EXAMPLE_ADC_VAL_RAM;
static uint16_t DCI_VALID_BITS_USERS_RAM = 1U;
static uint16_t DCI_USER_1_PWD_AGING_RAM = 1U;
static uint16_t DCI_USER_2_PWD_AGING_RAM = 0U;
static uint16_t DCI_USER_3_PWD_AGING_RAM = 0U;
static uint16_t DCI_USER_4_PWD_AGING_RAM = 0U;
static uint16_t DCI_USER_5_PWD_AGING_RAM = 0U;
static uint16_t DCI_USER_6_PWD_AGING_RAM = 0U;
static uint16_t DCI_IP_WORD_NOT_CONNECTED_RAM;
static uint16_t DCI_OP_WORD_NOT_CONNECTED_RAM;
static uint16_t DCI_PROFIBUS_PROD_MOD_DATA_RAM[100];
static uint16_t DCI_PRODUCT_ACTIVE_FAULT_RAM = 0U;
static uint16_t DCI_SAMPLE_APP_INPUT_WORD_RAM;
static uint16_t DCI_SAMPLE_APP_OUTPUT_WORD_RAM = 1U;
static uint16_t DCI_SAMPLE_ACYCLIC_PARAMETER_2_RAM = 0x55AAU;
static uint16_t DCI_SAMPLE_ACYCLIC_PARAMETER_6_RAM = 0xAA55U;
static uint16_t DCI_CIP_DEVICE_IDENT_DEV_STATUS_RAM = 0x74U;
static uint16_t DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_RAM = 120U;
static uint16_t DCI_APP_FIRM_VER_RAM;
static uint16_t DCI_EIP_TEST_INPUT_ASM_RAM;
static uint16_t DCI_EIP_TEST_OUTPUT_ASM_RAM;
static uint16_t DCI_DYN_IN_ASM_SELECT_1_RAM = 1U;
static uint16_t DCI_DYN_IN_ASM_SELECT_2_RAM = 12U;
static uint16_t DCI_DYN_IN_ASM_SELECT_3_RAM = 2U;
static uint16_t DCI_DYN_IN_ASM_SELECT_4_RAM = 3U;
static uint16_t DCI_DYN_IN_ASM_SELECT_5_RAM = 4U;
static uint16_t DCI_DYN_IN_ASM_SELECT_6_RAM = 6U;
static uint16_t DCI_DYN_IN_ASM_SELECT_7_RAM = 7U;
static uint16_t DCI_DYN_IN_ASM_SELECT_8_RAM = 8U;
static uint16_t DCI_DYN_OUT_ASM_SELECT_1_RAM = 1U;
static uint16_t DCI_DYN_OUT_ASM_SELECT_2_RAM = 2U;
static uint16_t DCI_DYN_OUT_ASM_SELECT_3_RAM = 3U;
static uint16_t DCI_DYN_OUT_ASM_SELECT_4_RAM = 4U;
static uint16_t DCI_LTK_FAULT_QUEUE_SORTED_RAM[10];
static uint16_t DCI_LTK_FAULT_QUEUE_EVENT_ORDER_RAM[10];
static uint16_t DCI_EIP_TEST1_RAM;
static uint16_t DCI_EIP_TEST2_RAM;
static uint16_t DCI_EIP_TEST3_RAM;
static uint16_t DCI_EIP_TEST4_RAM;
static uint16_t DCI_BACNET_IP_UPD_PORT_NUM_RAM = 47808U;
static uint16_t DCI_BACNET_IP_BBMD_PORT_RAM = 47808U;
static uint16_t DCI_BACNET_IP_REGISTRATION_INTERVAL_RAM = 10U;
static uint16_t DCI_BACNET_IP_COMM_TIMEOUT_RAM = 0U;
static uint16_t DCI_BACNET_MSTP_COMM_TIMEOUT_RAM = 10000U;
static uint16_t DCI_BACNET_MSTP_PARITY_RAM = 2U;
static uint16_t DCI_BACNET_MSTP_STOPBIT_RAM = 0U;
static uint16_t DCI_ANALOG_VALUE_0_RAM = 11U;
static uint16_t DCI_ANALOG_VALUE_1_RAM = 22U;
static uint16_t DCI_AV_ANY_PARAM_NUM_RAM = 0U;
static uint16_t DCI_ANALOG_INPUT_0_RAM = 33U;
static uint16_t DCI_ANALOG_INPUT_1_RAM = 44U;
static uint16_t DCI_DIGITAL_INPUT_WORD_RAM = 55U;
static uint16_t DCI_DIGITAL_OUTPUT_WORD_RAM = 66U;
static uint16_t DCI_SNTP_RETRY_TIME_RAM = 15000U;
static uint16_t DCI_IOT_PROXY_PORT_RAM = 8080U;
static uint16_t DCI_BLE_DEVICE_BONDING_INFO_RAM = 0U;
static uint16_t DCI_BLE_MIN_VAL_CONN_INTERVAL_RAM = 16U;
static uint16_t DCI_BLE_MAX_VAL_CONN_INTERVAL_RAM = 32U;
static uint16_t DCI_BLE_SLAVE_LATENCY_RAM = 4U;
static uint16_t DCI_BLE_SUPERVISION_TIMEOUT_RAM = 200U;
static uint16_t DCI_BLE_TEST_SETTING_A_RAM = 0x5634U;
static uint16_t DCI_BLE_TEST_SETTING_E_RAM = 5U;
static uint16_t DCI_BLE_TEST_PRESENT_STATUS_RAM = 500U;
static uint16_t DCI_USER_INACTIVITY_TIMEOUT_RAM = 900U;
static uint16_t DCI_USER_LOCK_TIME_SEC_RAM = 300U;
static uint16_t DCI_MAX_FAILED_LOGIN_ATTEMPTS_RAM = 10U;
static uint16_t DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_RAM = 0U;
static uint16_t DCI_MDNS_SERVER1_PORT_RAM = 0U;
static uint16_t DCI_J1939_MSG_EX4_RAM = 0U;
static uint16_t DCI_J1939_MSG_EX5_RAM = 0U;
static uint16_t DCI_J1939_MSG_EX6_RAM = 0U;
static uint16_t DCI_J1939_MSG_EX8_RAM = 0U;
static uint16_t DCI_BLE_TEST_COUNTER_RAM = 0U;
static uint16_t DCI_IEC61850_VLAN_VID1_RAM = 2U;
static uint16_t DCI_IEC61850_IND1_Q_RAM;
static uint16_t DCI_IEC61850_IND2_Q_RAM;
static uint16_t DCI_IEC61850_VLAN_VID2_RAM = 2U;
static uint16_t DCI_IEC61850_IND3_Q_RAM;
static uint16_t DCI_IEC61850_IND4_Q_RAM;
static uint16_t DCI_IEC61850_VLAN_VID4_RAM = 2U;
static uint16_t DCI_IEC61850_ALM1_Q_RAM;
static uint16_t DCI_IEC61850_ALM2_Q_RAM;
static uint16_t DCI_IEC61850_VLAN_VID5_RAM = 2U;
static uint16_t DCI_IEC61850_ALM5_Q_RAM;
static uint16_t DCI_IEC61850_ALM6_Q_RAM;
static uint16_t DCI_IEC61850_SAV_VLAN_VID1_RAM = 0U;
static uint16_t DCI_IEC61850_SAV_VLAN_APP_ID1_RAM = 0x4001U;
static uint16_t DCI_IEC61850_SAV_SMP_RATE1_RAM = 256U;
static uint16_t DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC1_RAM = 60U;
static uint16_t DCI_IEC61850_SAV_NUM_ASDU1_RAM = 8U;
static uint16_t DCI_IEC61850_SAV_PHA_AMP1_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHB_AMP1_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHC_AMP1_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHN_AMP1_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHA_VOLT1_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHB_VOLT1_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHC_VOLT1_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHN_VOLT1_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_VLAN_VID2_RAM = 0U;
static uint16_t DCI_IEC61850_SAV_VLAN_APP_ID2_RAM = 0x5001U;
static uint16_t DCI_IEC61850_SAV_SMP_SYNC2_RAM = 0U;
static uint16_t DCI_IEC61850_SAV_SMP_MODE2_RAM = 1U;
static uint16_t DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC2_RAM = 60U;
static uint16_t DCI_IEC61850_SAV_PHA_AMP2_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHB_AMP2_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHC_AMP2_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHN_AMP2_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHA_VOLT2_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHB_VOLT2_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHC_VOLT2_Q_RAM[8];
static uint16_t DCI_IEC61850_SAV_PHN_VOLT2_Q_RAM[8];
static uint16_t DCI_NV_MAP_VER_NUM_RAM = CONST_DCI_NV_MAP_VERSION_NUM;
static int16_t DCI_TIME_OFFSET_RAM = 0;
static int16_t DCI_BLE_TEST_SETTING_5_RAM = -500;
static int16_t DCI_IEC61850_VLAN_APP_ID1_RAM = 1001;
static int16_t DCI_IEC61850_VLAN_APP_ID2_RAM = 1002;
static int16_t DCI_IEC61850_VLAN_APP_ID4_RAM = 2001;
static int16_t DCI_IEC61850_VLAN_APP_ID5_RAM = 2002;
static int16_t DCI_IEC61850_SAV_SMP_MODE1_RAM = 1;
static int16_t DCI_IEC61850_SAV_SMP_RATE2_RAM = 256;
static uint8_t DCI_PRODUCT_NAME_RAM[32] = 
        { "SmartBreaker-2.0" };
static uint8_t DCI_USER_APP_NAME_RAM[32] = 
        { "SmartBreaker-2.0" };
static uint8_t DCI_FW_UPGRADE_MODE_RAM = 0U;
static uint8_t DCI_LAST_APP_FIRM_UPGRADE_USER_RAM[21] = 
        { "admin" };
static uint8_t DCI_LAST_WEB_FIRM_UPGRADE_USER_RAM[21] = 
        { "admin" };
static uint8_t DCI_WEB_FIRM_UPGRADE_STATUS_RAM = 0U;
static uint8_t DCI_HARDWARE_VER_ASCII_RAM[14];
static uint8_t DCI_HARDWARE_ID_VAL_RAM;
static uint8_t DCI_CSTACK_USAGE_PERCENT_RAM;
static uint8_t DCI_HEAP_USED_MAX_PERC_RAM = 0U;
static uint8_t DCI_HEAP_USED_PERC_RAM;
static uint8_t DCI_RESET_CAUSE_RAM;
static uint8_t DCI_ETHERNET_MAC_ADDRESS_RAM[6] = 
        { "" };
static uint8_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_RAM = 1U;
static uint8_t DCI_IP_ADDRESS_ALLOCATION_METHOD_RAM = 0U;
static uint8_t DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_RAM;
static uint8_t DCI_ACTIVE_IP_ADDRESS_RAM[4];
static uint8_t DCI_ACTIVE_SUBNET_MASK_RAM[4];
static uint8_t DCI_ACTIVE_DEFAULT_GATEWAY_RAM[4];
static uint8_t DCI_STATIC_IP_ADDRESS_RAM[4] = 
        { 0xFEU, 0x01U, 0xA8U, 0xC0U };
static uint8_t DCI_STATIC_SUBNET_MASK_RAM[4] = 
        { 0x00U, 0xFFU, 0xFFU, 0xFFU };
static uint8_t DCI_STATIC_DEFAULT_GATEWAY_RAM[4] = 
        { 0x01U, 0x01U, 0xA8U, 0xC0U };
static uint8_t DCI_PHY1_AUTONEG_STATE_RAM;
static uint8_t DCI_PHY2_AUTONEG_STATE_RAM;
static uint8_t DCI_ETH_ACD_CONFLICT_STATE_RAM;
static uint8_t DCI_ETH_ACD_CONFLICTED_STATE_RAM = 0U;
static uint8_t DCI_ETH_ACD_CONFLICTED_MAC_RAM[6] = 
        { 0U, 0U, 0U, 0U, 0U, 0U };
static uint8_t DCI_ETH_ACD_CONFLICTED_ARP_PDU_RAM[28] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static uint8_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_RAM = 0U;
static uint8_t DCI_VALID_BITS_ASSY_0_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_1_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_2_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_3_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_4_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_5_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_6_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_7_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_8_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_VALID_BITS_ASSY_9_RAM[2] = 
        { 0U, 0U };
static uint8_t DCI_EXAMPLE_PWM_DUTYCYCLE_RAM = 50U;
static uint8_t DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_RAM = 1U;
static uint8_t DCI_USER_WEB_FW_NAME_RAM[32] = 
        { "RTK_WEB_IMAGE" };
static uint8_t DCI_PRODUCT_PROC_NAME_RAM[32] = 
        { "RTK_MAIN_PROCESSOR" };
static uint8_t DCI_USER_1_FULLNAME_RAM[33] = 
        { "Administrator" };
static uint8_t DCI_USER_1_USERNAME_RAM[21] = 
        { "admin" };
static uint8_t DCI_USER_1_PWD_RAM[21] = 
        { "" };
static uint8_t DCI_USER_1_ROLE_RAM = 99U;
static uint8_t DCI_USER_1_PWD_COMPLEXITY_RAM = 0U;
static uint8_t DCI_USER_2_FULLNAME_RAM[33] = 
        { "" };
static uint8_t DCI_USER_2_USERNAME_RAM[21] = 
        { "" };
static uint8_t DCI_USER_2_PWD_RAM[21] = 
        { "" };
static uint8_t DCI_USER_2_ROLE_RAM = 0U;
static uint8_t DCI_USER_2_PWD_COMPLEXITY_RAM = 0U;
static uint8_t DCI_USER_3_FULLNAME_RAM[33] = 
        { "" };
static uint8_t DCI_USER_3_USERNAME_RAM[21] = 
        { "" };
static uint8_t DCI_USER_3_PWD_RAM[21] = 
        { "" };
static uint8_t DCI_USER_3_ROLE_RAM = 0U;
static uint8_t DCI_USER_3_PWD_COMPLEXITY_RAM = 0U;
static uint8_t DCI_USER_4_FULLNAME_RAM[33] = 
        { "" };
static uint8_t DCI_USER_4_USERNAME_RAM[21] = 
        { "" };
static uint8_t DCI_USER_4_PWD_RAM[21] = 
        { "" };
static uint8_t DCI_USER_4_ROLE_RAM = 0U;
static uint8_t DCI_USER_4_PWD_COMPLEXITY_RAM = 0U;
static uint8_t DCI_USER_5_FULLNAME_RAM[33] = 
        { "" };
static uint8_t DCI_USER_5_USERNAME_RAM[21] = 
        { "" };
static uint8_t DCI_USER_5_PWD_RAM[21] = 
        { "" };
static uint8_t DCI_USER_5_ROLE_RAM = 0U;
static uint8_t DCI_USER_5_PWD_COMPLEXITY_RAM = 0U;
static uint8_t DCI_USER_6_FULLNAME_RAM[33] = 
        { "" };
static uint8_t DCI_USER_6_USERNAME_RAM[21] = 
        { "" };
static uint8_t DCI_USER_6_PWD_RAM[21] = 
        { "" };
static uint8_t DCI_USER_6_ROLE_RAM = 0U;
static uint8_t DCI_USER_6_PWD_COMPLEXITY_RAM = 0U;
static uint8_t DCI_STATUS_WARNING_BITS_RAM[4];
static uint8_t DCI_SAMPLE_APP_INPUT_BIT_RAM;
static uint8_t DCI_SAMPLE_APP_OUTPUT_BIT_RAM = 1U;
static uint8_t DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_RAM = 1U;
static uint8_t DCI_SAMPLE_APP_PARAMETER_RAM = 1U;
static uint8_t DCI_SAMPLE_ACYCLIC_PARAMETER_3_RAM = 0x11U;
static uint8_t DCI_SAMPLE_ACYCLIC_PARAMETER_4_RAM = 0x22U;
static uint8_t DCI_ETH_DOMAIN_NAME_RAM[48] = 
        { "" };
static uint8_t DCI_ETH_HOST_NAME_RAM[64] = 
        { "" };
static uint8_t DCI_CIP_DEVICE_IDENT_STATE_RAM = 3U;
static uint8_t DCI_DYN_IN_ASM_INTERFACE_RAM;
static uint8_t DCI_DYN_OUT_ASM_INTERFACE_RAM;
static uint8_t DCI_LTK_FAULT_STATUS_BITS_RAM;
static uint8_t DCI_FAULT_RESET_RAM;
static uint8_t DCI_MAX_MASTER_RAM = 127U;
static uint8_t DCI_MAX_INFO_FRAMES_RAM = 1U;
static uint8_t DCI_BACNET_IP_FOREIGN_DEVICE_RAM = 0U;
static uint8_t DCI_BACNET_IP_BBMD_IP_RAM[4] = 
        { 0U, 0U, 0U, 0U };
static uint8_t DCI_BACNET_IP_PROTOCOL_STATUS_RAM;
static uint8_t DCI_BACNET_IP_FAULT_BEHAVIOR_RAM = 0U;
static uint8_t DCI_ENABLE_BACNET_IP_RAM = 0U;
static uint8_t DCI_BACNET_DEVICE_ADDR_RAM = 1U;
static uint8_t DCI_BACNET_MSTP_PROTOCOL_STATUS_RAM;
static uint8_t DCI_BACNET_FAULT_CODE_RAM;
static uint8_t DCI_MULTI_STATE_0_RAM = 0U;
static uint8_t DCI_MULTI_STATE_1_RAM = 0U;
static uint8_t DCI_TRUSTED_IP_WHITELIST_RAM[12] = 
        { 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0x01U, 0xA8U, 0xC0U, 0xFFU, 0xFFU, 0xFFU, 0xFFU };
static uint8_t DCI_DATE_FORMAT_RAM = 0U;
static uint8_t DCI_TIME_FORMAT_RAM = 1U;
static uint8_t DCI_REST_RESET_COMMAND_RAM;
static uint8_t DCI_DEVICE_CERT_VALID_RAM = 0U;
static uint8_t DCI_DEVICE_CERT_CONTROL_RAM = 1U;
static uint8_t DCI_SNTP_SERVICE_ENABLE_RAM = 1U;
static uint8_t DCI_SNTP_SERVER_1_RAM[40] = 
        { "pool.ntp.org" };
static uint8_t DCI_SNTP_SERVER_2_RAM[40] = 
        { "151.110.232.100" };
static uint8_t DCI_SNTP_SERVER_3_RAM[40] = 
        { "151.110.232.100" };
static uint8_t DCI_SNTP_ACTIVE_SERVER_RAM;
static uint8_t DCI_IOT_PROXY_SERVER_RAM[20] = 
        { "proxy.apac.etn.com" };
static uint8_t DCI_IOT_PROXY_USERNAME_RAM[21] = 
        { "" };
static uint8_t DCI_IOT_PROXY_PASSWORD_RAM[21] = 
        { "" };
static uint8_t DCI_IOT_DEVICE_GUID_RAM[37] = 
        { "" };
static uint8_t DCI_IOT_DEVICE_PROFILE_RAM[37] = 
        { "58af4fd1-b1f8-ec11-b47a-0050f2f4e861" };
static uint8_t DCI_IOT_CONN_STRING_RAM[170] = 
        { "" };
static uint8_t DCI_IOT_UPDATE_RATE_RAM = 5U;
static uint8_t DCI_IOT_CADENCE_UPDATE_RATE_RAM = 5U;
static uint8_t DCI_IOT_CONN_STAT_REASON_RAM = 0U;
static uint8_t DCI_CHANGE_BASED_LOGGING_RAM = 0U;
static uint8_t DCI_BLE_DEVICE_SNAME_RAM[13] = 
        { "EATON_DEV_1" };
static uint8_t DCI_BLE_DEVICE_PROFILE_ID_RAM[17] = 
        { "BLE_DEMO_PROFILE" };
static uint8_t DCI_BLE_DEVICE_VENDOR_RAM[13] = 
        { "SILICON_LABS" };
static uint8_t DCI_BLE_DEVICE_FAMILY_RAM[7] = 
        { "BGM111" };
static uint8_t DCI_BLE_DEVICE_ROLE_RAM[11] = 
        { "PERIPHERAL" };
static uint8_t DCI_BLE_DEVICE_MODEL_RAM[8] = 
        { "BLE_4_2" };
static uint8_t DCI_BLE_DEVICE_LNAME_RAM[18] = 
        { "EATON CORPORATION" };
static uint8_t DCI_BLE_DEVICE_HW_VER_RAM[8] = 
        { "REV_A03" };
static uint8_t DCI_BLE_DEVICE_SW_VER_RAM[10] = 
        { "SDK_2_4_2" };
static uint8_t DCI_BLE_USERNAME_RAM[32] = 
        { "ADMINISTRATOR" };
static uint8_t DCI_BLE_PASSWORD_RAM[32] = 
        { "ADMINISTRATOR" };
static uint8_t DCI_BLE_DEVICE_MANUFACTURER_NAME_RAM[11] = 
        { "BLUE GECKO" };
static uint8_t DCI_BLE_DEVICE_SERIAL_NO_RAM[10] = 
        { "172230625" };
static uint8_t DCI_BLE_DEVICE_IMAGE_URL_RAM[8] = 
        { "Desktop" };
static uint8_t DCI_BLE_ACTIVE_ACCESS_LEVEL_RAM = 7U;
static uint8_t DCI_BLE_TEST_SETTING_1_RAM = 0x45U;
static uint8_t DCI_BLE_TEST_SETTING_3_RAM[6] = 
        { 0xA8U, 0xC0U, 0xADU, 0xABU, 0xEDU, 0x3BU };
static uint8_t DCI_BLE_TEST_SETTING_D_RAM[4] = 
        { 0xA8U, 0xC0U, 0xADU, 0xABU };
static uint8_t DCI_BLE_TEST_SETTING_F_RAM = 0U;
static uint8_t DCI_BLE_TEST_READING_1_RAM[32] = 
        { "TEST STRING" };
static uint8_t DCI_CPU_UTILIZATION_RAM = 0U;
static uint8_t DCI_LOGGING_EVENT_CODE_RAM = 0U;
static uint8_t DCI_DEVICE_STATE_RAM = 0U;
static uint8_t DCI_NV_MEM_ID_RAM;
static uint8_t DCI_NV_FAIL_OPERATION_RAM;
static uint8_t DCI_LOG_PROC_IMAGE_ID_RAM;
static uint8_t DCI_LOG_FIRMWARE_VERSION_RAM[14];
static uint8_t DCI_LOGGED_USERNAME_RAM[21] = 
        { "" };
static uint8_t DCI_LOG_IP_ADDRESS_RAM[4];
static uint8_t DCI_LOG_PORT_NUMBER_RAM;
static uint8_t DCI_BLE_ADMIN_PASSWORD_RAM[32] = 
        { "ADMINISTRATOR" };
static uint8_t DCI_BLE_OBSERVER_PASSWORD_RAM[32] = 
        { "OBSERVER" };
static uint8_t DCI_BLE_OPERATOR_PASSWORD_RAM[32] = 
        { "OPERATOR" };
static uint8_t DCI_BLE_ENGINEER_PASSWORD_RAM[32] = 
        { "ENGINEER" };
static uint8_t DCI_BLE_USER1_ROLE_RAM = 0U;
static uint8_t DCI_BLE_USER1_NAME_RAM[32] = 
        { "Observer" };
static uint8_t DCI_BLE_USER1_PASSWORD_RAM[28] = 
        { "observer" };
static uint8_t DCI_BLE_USER2_ROLE_RAM = 1U;
static uint8_t DCI_BLE_USER2_NAME_RAM[32] = 
        { "Operator" };
static uint8_t DCI_BLE_USER2_PASSWORD_RAM[28] = 
        { "operator" };
static uint8_t DCI_BLE_USER3_ROLE_RAM = 2U;
static uint8_t DCI_BLE_USER3_NAME_RAM[32] = 
        { "Engineer" };
static uint8_t DCI_BLE_USER4_ROLE_RAM = 3U;
static uint8_t DCI_RESET_FUNCTION_NAME_RAM[20] = 
        { "" };
static uint8_t DCI_RESET_OS_TASK_NAME_RAM[20] = 
        { "" };
static uint8_t DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_RAM[20] = 
        { "" };
static uint8_t DCI_MAX_CONCURRENT_SESSION_RAM = 1U;
static uint8_t DCI_CORS_TYPE_RAM = 0U;
static uint8_t DCI_BLE_EDDYSTONE_URL_RAM[18] = 
        { 0x03U, 0x45U, 0x61U, 0x74U, 0x6FU, 0x6EU, 0x07U, 0x21U };
static uint8_t DCI_BLE_IBEACON_MAJOR_NUMBER_RAM[2] = 
        { 0x11U, 0x22U };
static uint8_t DCI_BLE_IBEACON_MINOR_NUMBER_RAM[2] = 
        { 0x33U, 0x44U };
static uint8_t DCI_BLE_IBEACON_128_BIT_UUID_RAM[16] = 
        { 0xF7U, 0x82U, 0x6DU, 0xA6U, 0x4FU, 0xA2U, 0x4EU, 0x98U, 
0x80, 0x24U, 0xBCU, 0x5BU, 0x71U, 0xE0U, 0x89U, 0x3EU };
static uint8_t DCI_MDNS_SERVER1_NAME_RAM[64] = 
        { "PxGreenHost0" };
static uint8_t DCI_MDNS_SERVER1_IP_ADDR_RAM[4] = 
        { 0x00U, 0x00U, 0x00U, 0x00U };
static uint8_t DCI_J1939_MSG_EX1_RAM = 0U;
static uint8_t DCI_J1939_MSG_EX2_RAM = 0U;
static uint8_t DCI_J1939_MSG_EX3_RAM = 0U;
static uint8_t DCI_J1939_MSG_EX7_RAM = 0U;
static uint8_t DCI_DEVICE_PWD_SETTING_RAM = 0U;
static uint8_t DCI_DEVICE_DEFAULT_PWD_RAM[21] = 
        { "Admin*1" };
static uint8_t DCI_LANG_PREF_SETTING_RAM = 0U;
static uint8_t DCI_COMMON_LANG_PREF_RAM[6] = 
        { "en" };
static uint8_t DCI_LAST_LANG_FIRM_UPGRADE_USER_RAM[21] = 
        { "admin" };
static uint8_t DCI_BLE_LOG_RAM[130];
static uint8_t DCI_IEC61850_VLAN_PRI1_RAM = 2U;
static uint8_t DCI_IEC61850_MAC_ADD_ID1_RAM[6] = 
        { 0x01U, 0x0CU, 0xCDU, 0xBEU, 0xEFU, 0x11U };
static uint8_t DCI_IEC61850_GOOSE_ID1_RAM[32] = 
        { "goID_SubTest1" };
static uint8_t DCI_IEC61850_CB_REF_NAME1_RAM[32] = 
        { "Device_1/LLN0.gocbRef_1" };
static uint8_t DCI_IEC61850_DATA_SET_NAME1_RAM[32] = 
        { "Device_1/datset_name_1" };
static uint8_t DCI_IEC61850_NDS_COM1_RAM = 0U;
static uint8_t DCI_IEC61850_TEST1_RAM = 0U;
static uint8_t DCI_IEC61850_NUM_DATA_SET_ENTRIES1_RAM = 2U;
static uint8_t DCI_IEC61850_VLAN_PRI2_RAM = 2U;
static uint8_t DCI_IEC61850_MAC_ADD_ID2_RAM[6] = 
        { 0x01U, 0x0CU, 0xCDU, 0xBEU, 0xEFU, 0x22U };
static uint8_t DCI_IEC61850_GOOSE_ID2_RAM[32] = 
        { "goID_SubTest2" };
static uint8_t DCI_IEC61850_CB_REF_NAME2_RAM[32] = 
        { "Device_1/LLN0.gocbRef_2" };
static uint8_t DCI_IEC61850_DATA_SET_NAME2_RAM[32] = 
        { "Device_1/datset_name_2" };
static uint8_t DCI_IEC61850_NDS_COM2_RAM = 0U;
static uint8_t DCI_IEC61850_TEST2_RAM = 0U;
static uint8_t DCI_IEC61850_NUM_DATA_SET_ENTRIES2_RAM = 2U;
static uint8_t DCI_IEC61850_VLAN_PRI4_RAM = 2U;
static uint8_t DCI_IEC61850_MAC_ADD_ID4_RAM[6] = 
        { 0x01U, 0x0CU, 0xCDU, 0xBEU, 0xEFU, 0x21U };
static uint8_t DCI_IEC61850_GOOSE_ID4_RAM[32] = 
        { "goID_SubTest1" };
static uint8_t DCI_IEC61850_CB_REF_NAME4_RAM[32] = 
        { "Device_2/LLN0.gocbRef_1" };
static uint8_t DCI_IEC61850_DATA_SET_NAME4_RAM[32] = 
        { "Device_2/datset_name_1" };
static uint8_t DCI_IEC61850_NDS_COM4_RAM = 0U;
static uint8_t DCI_IEC61850_TEST4_RAM = 0U;
static uint8_t DCI_IEC61850_NUM_DATA_SET_ENTRIES4_RAM = 2U;
static uint8_t DCI_IEC61850_VLAN_PRI5_RAM = 2U;
static uint8_t DCI_IEC61850_MAC_ADD_ID5_RAM[6] = 
        { 0x01U, 0x0CU, 0xCDU, 0xBEU, 0xEFU, 0x22U };
static uint8_t DCI_IEC61850_GOOSE_ID5_RAM[32] = 
        { "goID_SubTest2" };
static uint8_t DCI_IEC61850_CB_REF_NAME5_RAM[32] = 
        { "Device_2/LLN0.gocbRef_2" };
static uint8_t DCI_IEC61850_DATA_SET_NAME5_RAM[32] = 
        { "Device_2/datset_name_2" };
static uint8_t DCI_IEC61850_NDS_COM5_RAM = 0U;
static uint8_t DCI_IEC61850_TEST5_RAM = 0U;
static uint8_t DCI_IEC61850_NUM_DATA_SET_ENTRIES5_RAM = 2U;
static uint8_t DCI_IEC61850_SAV_VLAN_PRI1_RAM = 4U;
static uint8_t DCI_IEC61850_SAV_MAC_ADD_ID1_RAM[6] = 
        { 0x01U, 0xFEU, 0xEDU, 0xBEU, 0xEFU, 0x11U };
static uint8_t DCI_IEC61850_SAV_ID1_RAM[16] = 
        { "MU01" };
static uint8_t DCI_IEC61850_SAV_CB_REF_NAME1_RAM[32] = 
        { "Device_1/LLN0.svbRef_1" };
static uint8_t DCI_IEC61850_SAV_DATA_SET_NAME1_RAM[16] = 
        { "PhsMeas1" };
static uint8_t DCI_IEC61850_SAV_SMP_SYNC1_RAM = 0U;
static uint8_t DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES1_RAM = 8U;
static uint8_t DCI_IEC61850_SAV_VLAN_PRI2_RAM = 4U;
static uint8_t DCI_IEC61850_SAV_MAC_ADD_ID2_RAM[6] = 
        { 0x01, 0xFE, 0xED, 0xBE, 0xEF, 0x21 };
static uint8_t DCI_IEC61850_SAV_ID2_RAM[16] = 
        { "MU02" };
static uint8_t DCI_IEC61850_SAV_CB_REF_NAME2_RAM[32] = 
        { "Device_2/LLN0.svbRef_1" };
static uint8_t DCI_IEC61850_SAV_DATA_SET_NAME2_RAM[16] = 
        { "PhsMeas2" };
static uint8_t DCI_IEC61850_SAV_CONFIG_REV2_RAM = 1U;
static uint8_t DCI_IEC61850_SAV_NUM_ASDU2_RAM = 8U;
static uint8_t DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES2_RAM = 8U;
static uint8_t DCI_CLIENT_AP_SSID_RAM[32] = 
        { "Temp" };
static uint8_t DCI_CLIENT_AP_PASSPHRASE_RAM[32] = 
        { "key12345" };
static uint8_t DCI_CLIENT_AP_CHANNEL_RAM = 6U;
static uint8_t DCI_CLIENT_AP_BAND_RAM = 1U;
static uint8_t DCI_CLIENT_AP_RETRY_COUNT_RAM = 10U;
static uint8_t DCI_SOURCE_AP_SSID_RAM[32] = 
        { "Temp" };
static uint8_t DCI_SOURCE_AP_PASSPHRASE_RAM[33] = 
        { "key12345" };
static uint8_t DCI_SOURCE_AP_CHANNEL_RAM = 6U;
static uint8_t DCI_SOURCE_AP_MAX_CONN_RAM = 4U;
static uint8_t DCI_DATA_LOG_SPEC_VERSION_RAM = 1U;
static uint8_t DCI_EVENT_LOG_SPEC_VERSION_RAM = 1U;
static uint8_t DCI_AUDIT_POWER_LOG_SPEC_VERSION_RAM = 1U;
static uint8_t DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_RAM = 1U;
static uint8_t DCI_AUDIT_USER_LOG_SPEC_VERSION_RAM = 1U;
static uint8_t DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_RAM = 1U;
static uint8_t DCI_IOT_NetworkSSID_RAM[32] = 
        { "" };
static uint8_t DCI_IOT_NetworkPassword_RAM[32] = 
        { "" };
static uint8_t DCI_IOT_MacAddress_RAM[20] = 
        { "" };
static uint8_t DCI_BUILD_TYPE_RAM[32] = 
        { "" };
static uint8_t DCI_NEW_PROG_PART_NUMBER_RAM[32] = 
        { "" };
static uint8_t DCI_IOT_UNICAST_PRIMARY_UDP_KEY_RAM[48] = 
        { "" };
static uint8_t DCI_IOT_GET_UNICAST_UDP_KEY_RAM[32] = 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static uint8_t DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_RAM[48] = 
        { "" };
static uint8_t DCI_IOT_GET_BROADCAST_UDP_KEY_RAM[32] = 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static uint8_t DCI_IOT_UNICAST_SECONDARY_UDP_KEY_RAM[48] = 
        { "" };
static uint8_t DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_RAM[32] = 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static uint8_t DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_RAM[48] = 
        { "" };
static uint8_t DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_RAM[32] = 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static uint8_t DCI_IOT_ErrorLogData_RAM[100] = 
        { "" };
static uint8_t DCI_IOT_PATH_STATUS_RAM = 0U;
static uint8_t DCI_IOT_PRIMARY_STATE_RAM = 0U;
static uint8_t DCI_IOT_SECONDARY_STATE_RAM = 0U;
static uint8_t DCI_IOT_DPS_ENDPOINT_RAM[80];
static uint8_t DCI_IOT_DPS_ID_SCOPE_RAM[20];
static uint8_t DCI_IOT_DPS_SYMM_KEY_RAM[45];
static uint8_t DCI_IOT_DPS_DEVICE_REG_ID_RAM[37];
static uint8_t DCI_IOT_IDENTIFY_ME_RAM = 0U;
static uint8_t DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_RAM = 0U;
static int8_t DCI_BLE_TEST_SETTING_2_RAM = -15;
static int8_t DCI_BLE_TX_POWER_RAM = -4;
static dci_bool_t DCI_PHY1_FULL_DUPLEX_ENABLE_RAM = True;
static dci_bool_t DCI_PHY1_FULL_DUPLEX_ACTUAL_RAM;
static dci_bool_t DCI_PHY1_AUTONEG_ENABLE_RAM = True;
static dci_bool_t DCI_PHY1_PORT_ENABLE_RAM = True;
static dci_bool_t DCI_PHY2_FULL_DUPLEX_ENABLE_RAM = True;
static dci_bool_t DCI_PHY2_FULL_DUPLEX_ACTUAL_RAM;
static dci_bool_t DCI_PHY2_AUTONEG_ENABLE_RAM = True;
static dci_bool_t DCI_PHY2_PORT_ENABLE_RAM = True;
static dci_bool_t DCI_ETH_ACD_ENABLE_RAM = True;
static dci_bool_t DCI_HTTP_ENABLE_RAM = True;
static dci_bool_t DCI_IP_BIT_NOT_CONNECTED_RAM;
static dci_bool_t DCI_OP_BIT_NOT_CONNECTED_RAM;
static dci_bool_t DCI_DEVICE_PARAMETERIZATION_ENABLE_RAM = True;
static dci_bool_t DCI_INPUT_TEST_BIT_1_RAM;
static dci_bool_t DCI_OUTPUT_TEST_BIT_1_RAM;
static dci_bool_t DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_RAM = False;
static dci_bool_t DCI_DIGITAL_INPUT_BIT_RAM;
static dci_bool_t DCI_DIGITAL_OUTPUT_BIT_RAM = False;
static dci_bool_t DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_RAM = True;
static dci_bool_t DCI_TRUSTED_IP_BACNET_IP_ENABLE_RAM = True;
static dci_bool_t DCI_TRUSTED_IP_EIP_ENABLE_RAM = True;
static dci_bool_t DCI_IOT_CONNECT_RAM = True;
static dci_bool_t DCI_IOT_STATUS_RAM = False;
static dci_bool_t DCI_IOT_PROXY_ENABLE_RAM = False;
static dci_bool_t DCI_MANUAL_LOG_TRIGGER_RAM;
static dci_bool_t DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_RAM = False;
static dci_bool_t DCI_BLE_TEST_SETTING_4_RAM = False;
static dci_bool_t DCI_LOG_EVENT_TEST_101_RAM;
static dci_bool_t DCI_LOG_EVENT_TEST_102_RAM;
static dci_bool_t DCI_MODBUS_TCP_ENABLE_RAM = True;
static dci_bool_t DCI_BACNET_IP_ENABLE_RAM = True;
static dci_bool_t DCI_ENABLE_SESSION_TAKEOVER_RAM = 1U;
static dci_bool_t DCI_MDNS_LWM2M_SERVICE_ENABLE_RAM = False;
static dci_bool_t DCI_PTP_ENABLE_RAM = False;
static dci_bool_t DCI_IEC61850_IND1_STVAL_RAM;
static dci_bool_t DCI_IEC61850_IND2_STVAL_RAM;
static dci_bool_t DCI_IEC61850_IND3_STVAL_RAM;
static dci_bool_t DCI_IEC61850_IND4_STVAL_RAM;
static dci_bool_t DCI_IEC61850_ALM1_STVAL_RAM;
static dci_bool_t DCI_IEC61850_ALM2_STVAL_RAM;
static dci_bool_t DCI_IEC61850_ALM5_STVAL_RAM;
static dci_bool_t DCI_IEC61850_ALM6_STVAL_RAM;
static dci_bool_t DCI_IOT_SECONDARY_CONTACT_STATUS_RAM = 0U;
static dci_bool_t DCI_IOT_PRIMARY_CONTACT_STATUS_RAM = 0U;
static dci_bool_t DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_RAM = 0U;
static dci_bool_t DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_RAM = 0U;
static dci_bool_t DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_RAM = 0U;
static dci_bool_t DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_RAM = 0U;
static dci_bool_t DCI_IOT_SBLCP_EN_DIS_RAM = 0U;


//*******************************************************
//******     The Callback Handler index
//*******************************************************
static const DCI_CBACK_INDEX_TD DCI_PRODUCT_SERIAL_NUM_CBACK_INDEX = 0U;
static const DCI_CBACK_INDEX_TD DCI_PRODUCT_NAME_CBACK_INDEX = 1U;
static const DCI_CBACK_INDEX_TD DCI_USER_APP_NAME_CBACK_INDEX = 2U;
static const DCI_CBACK_INDEX_TD DCI_HARDWARE_VER_ASCII_CBACK_INDEX = 3U;
static const DCI_CBACK_INDEX_TD DCI_ETHERNET_MAC_ADDRESS_CBACK_INDEX = 4U;
static const DCI_CBACK_INDEX_TD DCI_IP_ADDRESS_ALLOCATION_METHOD_CBACK_INDEX = 5U;
static const DCI_CBACK_INDEX_TD DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_CBACK_INDEX = 6U;
static const DCI_CBACK_INDEX_TD DCI_SIMPLE_APP_CONTROL_WORD_CBACK_INDEX = 7U;
static const DCI_CBACK_INDEX_TD DCI_PRODUCT_PROC_SERIAL_NUM_CBACK_INDEX = 8U;
static const DCI_CBACK_INDEX_TD DCI_USER_1_PWD_CBACK_INDEX = 9U;
static const DCI_CBACK_INDEX_TD DCI_USER_2_PWD_CBACK_INDEX = 10U;
static const DCI_CBACK_INDEX_TD DCI_USER_3_PWD_CBACK_INDEX = 11U;
static const DCI_CBACK_INDEX_TD DCI_USER_4_PWD_CBACK_INDEX = 12U;
static const DCI_CBACK_INDEX_TD DCI_USER_5_PWD_CBACK_INDEX = 13U;
static const DCI_CBACK_INDEX_TD DCI_USER_6_PWD_CBACK_INDEX = 14U;
static const DCI_CBACK_INDEX_TD DCI_EIP_TEST_INPUT_ASM_CBACK_INDEX = 15U;
static const DCI_CBACK_INDEX_TD DCI_EIP_TEST_OUTPUT_ASM_CBACK_INDEX = 16U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_INTERFACE_CBACK_INDEX = 17U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_SELECT_1_CBACK_INDEX = 18U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_SELECT_2_CBACK_INDEX = 19U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_SELECT_3_CBACK_INDEX = 20U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_SELECT_4_CBACK_INDEX = 21U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_SELECT_5_CBACK_INDEX = 22U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_SELECT_6_CBACK_INDEX = 23U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_SELECT_7_CBACK_INDEX = 24U;
static const DCI_CBACK_INDEX_TD DCI_DYN_IN_ASM_SELECT_8_CBACK_INDEX = 25U;
static const DCI_CBACK_INDEX_TD DCI_DYN_OUT_ASM_INTERFACE_CBACK_INDEX = 26U;
static const DCI_CBACK_INDEX_TD DCI_DYN_OUT_ASM_SELECT_1_CBACK_INDEX = 27U;
static const DCI_CBACK_INDEX_TD DCI_DYN_OUT_ASM_SELECT_2_CBACK_INDEX = 28U;
static const DCI_CBACK_INDEX_TD DCI_DYN_OUT_ASM_SELECT_3_CBACK_INDEX = 29U;
static const DCI_CBACK_INDEX_TD DCI_DYN_OUT_ASM_SELECT_4_CBACK_INDEX = 30U;
static const DCI_CBACK_INDEX_TD DCI_FAULT_RESET_CBACK_INDEX = 31U;
static const DCI_CBACK_INDEX_TD DCI_AV_ANY_PARAM_NUM_CBACK_INDEX = 32U;
static const DCI_CBACK_INDEX_TD DCI_AV_ANY_PARAM_VAL_CBACK_INDEX = 33U;
static const DCI_CBACK_INDEX_TD DCI_UNIX_EPOCH_TIME_CBACK_INDEX = 34U;
static const DCI_CBACK_INDEX_TD DCI_UNIX_EPOCH_TIME_64_BIT_CBACK_INDEX = 35U;
static const DCI_CBACK_INDEX_TD DCI_REST_RESET_COMMAND_CBACK_INDEX = 36U;
static const DCI_CBACK_INDEX_TD DCI_DEVICE_CERT_CONTROL_CBACK_INDEX = 37U;
static const DCI_CBACK_INDEX_TD DCI_SNTP_SERVICE_ENABLE_CBACK_INDEX = 38U;
static const DCI_CBACK_INDEX_TD DCI_SNTP_SERVER_1_CBACK_INDEX = 39U;
static const DCI_CBACK_INDEX_TD DCI_SNTP_SERVER_2_CBACK_INDEX = 40U;
static const DCI_CBACK_INDEX_TD DCI_SNTP_SERVER_3_CBACK_INDEX = 41U;
static const DCI_CBACK_INDEX_TD DCI_SNTP_UPDATE_TIME_CBACK_INDEX = 42U;
static const DCI_CBACK_INDEX_TD DCI_SNTP_RETRY_TIME_CBACK_INDEX = 43U;
static const DCI_CBACK_INDEX_TD DCI_IOT_CONNECT_CBACK_INDEX = 44U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PROXY_ENABLE_CBACK_INDEX = 45U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PROXY_SERVER_CBACK_INDEX = 46U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PROXY_PORT_CBACK_INDEX = 47U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PROXY_USERNAME_CBACK_INDEX = 48U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PROXY_PASSWORD_CBACK_INDEX = 49U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DEVICE_GUID_CBACK_INDEX = 50U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DEVICE_PROFILE_CBACK_INDEX = 51U;
static const DCI_CBACK_INDEX_TD DCI_IOT_CONN_STRING_CBACK_INDEX = 52U;
static const DCI_CBACK_INDEX_TD DCI_GROUP0_CADENCE_CBACK_INDEX = 53U;
static const DCI_CBACK_INDEX_TD DCI_GROUP1_CADENCE_CBACK_INDEX = 54U;
static const DCI_CBACK_INDEX_TD DCI_GROUP2_CADENCE_CBACK_INDEX = 55U;
static const DCI_CBACK_INDEX_TD DCI_GROUP3_CADENCE_CBACK_INDEX = 56U;
static const DCI_CBACK_INDEX_TD DCI_MANUAL_LOG_TRIGGER_CBACK_INDEX = 57U;
static const DCI_CBACK_INDEX_TD DCI_BLE_USERNAME_CBACK_INDEX = 58U;
static const DCI_CBACK_INDEX_TD DCI_BLE_PASSWORD_CBACK_INDEX = 59U;
static const DCI_CBACK_INDEX_TD DCI_BLE_ADMIN_PASSWORD_CBACK_INDEX = 60U;
static const DCI_CBACK_INDEX_TD DCI_BLE_OBSERVER_PASSWORD_CBACK_INDEX = 61U;
static const DCI_CBACK_INDEX_TD DCI_BLE_OPERATOR_PASSWORD_CBACK_INDEX = 62U;
static const DCI_CBACK_INDEX_TD DCI_BLE_ENGINEER_PASSWORD_CBACK_INDEX = 63U;
static const DCI_CBACK_INDEX_TD DCI_BLE_USER1_PASSWORD_CBACK_INDEX = 64U;
static const DCI_CBACK_INDEX_TD DCI_BLE_USER2_PASSWORD_CBACK_INDEX = 65U;
static const DCI_CBACK_INDEX_TD DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_CBACK_INDEX = 66U;
static const DCI_CBACK_INDEX_TD DCI_MDNS_LWM2M_SERVICE_ENABLE_CBACK_INDEX = 67U;
static const DCI_CBACK_INDEX_TD DCI_MDNS_SERVER1_IP_ADDR_CBACK_INDEX = 68U;
static const DCI_CBACK_INDEX_TD DCI_DEVICE_DEFAULT_PWD_CBACK_INDEX = 69U;
static const DCI_CBACK_INDEX_TD DCI_BLE_LOG_CBACK_INDEX = 70U;
static const DCI_CBACK_INDEX_TD DCI_BLE_LOG_INDEX_CBACK_INDEX = 71U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND1_STVAL_CBACK_INDEX = 72U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND1_Q_CBACK_INDEX = 73U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND1_T_CBACK_INDEX = 74U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND2_STVAL_CBACK_INDEX = 75U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND2_Q_CBACK_INDEX = 76U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND2_T_CBACK_INDEX = 77U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND3_STVAL_CBACK_INDEX = 78U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND3_Q_CBACK_INDEX = 79U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND3_T_CBACK_INDEX = 80U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND4_STVAL_CBACK_INDEX = 81U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND4_Q_CBACK_INDEX = 82U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_IND4_T_CBACK_INDEX = 83U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM1_STVAL_CBACK_INDEX = 84U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM1_Q_CBACK_INDEX = 85U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM1_T_CBACK_INDEX = 86U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM2_STVAL_CBACK_INDEX = 87U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM2_Q_CBACK_INDEX = 88U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM2_T_CBACK_INDEX = 89U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM5_STVAL_CBACK_INDEX = 90U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM5_Q_CBACK_INDEX = 91U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM5_T_CBACK_INDEX = 92U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM6_STVAL_CBACK_INDEX = 93U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM6_Q_CBACK_INDEX = 94U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_ALM6_T_CBACK_INDEX = 95U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHA_AMP1_CBACK_INDEX = 96U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHA_AMP1_Q_CBACK_INDEX = 97U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHB_AMP1_CBACK_INDEX = 98U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHB_AMP1_Q_CBACK_INDEX = 99U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHC_AMP1_CBACK_INDEX = 100U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHC_AMP1_Q_CBACK_INDEX = 101U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHN_AMP1_CBACK_INDEX = 102U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHN_AMP1_Q_CBACK_INDEX = 103U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHA_VOLT1_CBACK_INDEX = 104U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHA_VOLT1_Q_CBACK_INDEX = 105U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHB_VOLT1_CBACK_INDEX = 106U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHB_VOLT1_Q_CBACK_INDEX = 107U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHC_VOLT1_CBACK_INDEX = 108U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHC_VOLT1_Q_CBACK_INDEX = 109U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHN_VOLT1_CBACK_INDEX = 110U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHN_VOLT1_Q_CBACK_INDEX = 111U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHA_AMP2_CBACK_INDEX = 112U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHA_AMP2_Q_CBACK_INDEX = 113U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHB_AMP2_CBACK_INDEX = 114U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHB_AMP2_Q_CBACK_INDEX = 115U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHC_AMP2_CBACK_INDEX = 116U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHC_AMP2_Q_CBACK_INDEX = 117U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHN_AMP2_CBACK_INDEX = 118U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHN_AMP2_Q_CBACK_INDEX = 119U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHA_VOLT2_CBACK_INDEX = 120U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHA_VOLT2_Q_CBACK_INDEX = 121U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHB_VOLT2_CBACK_INDEX = 122U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHB_VOLT2_Q_CBACK_INDEX = 123U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHC_VOLT2_CBACK_INDEX = 124U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHC_VOLT2_Q_CBACK_INDEX = 125U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHN_VOLT2_CBACK_INDEX = 126U;
static const DCI_CBACK_INDEX_TD DCI_IEC61850_SAV_PHN_VOLT2_Q_CBACK_INDEX = 127U;
static const DCI_CBACK_INDEX_TD DCI_CLIENT_AP_SSID_CBACK_INDEX = 128U;
static const DCI_CBACK_INDEX_TD DCI_CLIENT_AP_PASSPHRASE_CBACK_INDEX = 129U;
static const DCI_CBACK_INDEX_TD DCI_SOURCE_AP_SSID_CBACK_INDEX = 130U;
static const DCI_CBACK_INDEX_TD DCI_SOURCE_AP_PASSPHRASE_CBACK_INDEX = 131U;
static const DCI_CBACK_INDEX_TD DCI_DATA_LOG_SPEC_VERSION_CBACK_INDEX = 132U;
static const DCI_CBACK_INDEX_TD DCI_EVENT_LOG_SPEC_VERSION_CBACK_INDEX = 133U;
static const DCI_CBACK_INDEX_TD DCI_AUDIT_POWER_LOG_SPEC_VERSION_CBACK_INDEX = 134U;
static const DCI_CBACK_INDEX_TD DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_CBACK_INDEX = 135U;
static const DCI_CBACK_INDEX_TD DCI_AUDIT_USER_LOG_SPEC_VERSION_CBACK_INDEX = 136U;
static const DCI_CBACK_INDEX_TD DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_CBACK_INDEX = 137U;
static const DCI_CBACK_INDEX_TD DCI_IOT_SECONDARY_CONTACT_STATUS_CBACK_INDEX = 138U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ResetBreaker_CBACK_INDEX = 139U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PRIMARY_CONTACT_STATUS_CBACK_INDEX = 140U;
static const DCI_CBACK_INDEX_TD DCI_IOT_VoltageL1_CBACK_INDEX = 141U;
static const DCI_CBACK_INDEX_TD DCI_IOT_VoltageL2_CBACK_INDEX = 142U;
static const DCI_CBACK_INDEX_TD DCI_IOT_CurrentL1_CBACK_INDEX = 143U;
static const DCI_CBACK_INDEX_TD DCI_IOT_CurrentL2_CBACK_INDEX = 144U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ActivePowerL1_CBACK_INDEX = 145U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ActivePowerL2_CBACK_INDEX = 146U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReactivePowerL1_CBACK_INDEX = 147U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReactivePowerL2_CBACK_INDEX = 148U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ApparentPowerL1_CBACK_INDEX = 149U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ApparentPowerL2_CBACK_INDEX = 150U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ActiveEnergyL1_CBACK_INDEX = 151U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ActiveEnergyL2_CBACK_INDEX = 152U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReactiveEnergyL1_CBACK_INDEX = 153U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReactiveEnergyL2_CBACK_INDEX = 154U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ApparentEnergyL1_CBACK_INDEX = 155U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ApparentEnergyL2_CBACK_INDEX = 156U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PowerFactorL1_CBACK_INDEX = 157U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PowerFactorL2_CBACK_INDEX = 158U;
static const DCI_CBACK_INDEX_TD DCI_IOT_FrequencyL1_CBACK_INDEX = 159U;
static const DCI_CBACK_INDEX_TD DCI_IOT_FrequencyL2_CBACK_INDEX = 160U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReverseActiveEnergyL1_CBACK_INDEX = 161U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReverseReactiveEnergyL1_CBACK_INDEX = 162U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReverseApparentEnergyL1_CBACK_INDEX = 163U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReverseActiveEnergyL2_CBACK_INDEX = 164U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReverseReactiveEnergyL2_CBACK_INDEX = 165U;
static const DCI_CBACK_INDEX_TD DCI_IOT_ReverseApparentEnergyL2_CBACK_INDEX = 166U;
static const DCI_CBACK_INDEX_TD DCI_IOT_UNICAST_PRIMARY_UDP_KEY_CBACK_INDEX = 167U;
static const DCI_CBACK_INDEX_TD DCI_IOT_GET_UNICAST_UDP_KEY_CBACK_INDEX = 168U;
static const DCI_CBACK_INDEX_TD DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_CBACK_INDEX = 169U;
static const DCI_CBACK_INDEX_TD DCI_IOT_GET_BROADCAST_UDP_KEY_CBACK_INDEX = 170U;
static const DCI_CBACK_INDEX_TD DCI_IOT_UNICAST_SECONDARY_UDP_KEY_CBACK_INDEX = 171U;
static const DCI_CBACK_INDEX_TD DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_CBACK_INDEX = 172U;
static const DCI_CBACK_INDEX_TD DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_CBACK_INDEX = 173U;
static const DCI_CBACK_INDEX_TD DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_CBACK_INDEX = 174U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PATH_STATUS_CBACK_INDEX = 175U;
static const DCI_CBACK_INDEX_TD DCI_IOT_PRIMARY_STATE_CBACK_INDEX = 176U;
static const DCI_CBACK_INDEX_TD DCI_IOT_SECONDARY_STATE_CBACK_INDEX = 177U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_CBACK_INDEX = 178U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_CBACK_INDEX = 179U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_CBACK_INDEX = 180U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_CBACK_INDEX = 181U;
static const DCI_CBACK_INDEX_TD DCI_IOT_Load_Percentage_CBACK_INDEX = 182U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DPS_ENDPOINT_CBACK_INDEX = 183U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DPS_ID_SCOPE_CBACK_INDEX = 184U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DPS_SYMM_KEY_CBACK_INDEX = 185U;
static const DCI_CBACK_INDEX_TD DCI_IOT_DPS_DEVICE_REG_ID_CBACK_INDEX = 186U;
static const DCI_CBACK_INDEX_TD DCI_IOT_IDENTIFY_ME_CBACK_INDEX = 187U;
static const DCI_CBACK_INDEX_TD DCI_IOT_POWER_UP_STATE_CBACK_INDEX = 188U;
static const DCI_CBACK_INDEX_TD DCI_IOT_SBLCP_EN_DIS_CBACK_INDEX = 189U;


//*******************************************************
//******     The Callback Block
//*******************************************************
DCI_CBACK_BLOCK_TD  DCI_CBACK_ARRAY[TOTAL_CBACK_ARRAY_SIZE] = {
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
},
{
    nullptr,       //Callback Nulled
    nullptr,       //Callback Param Nulled
    static_cast<DCI_CB_MSK_TD>( 0U )        //Callback Mask
}
};


//*******************************************************
//******     The Range block index
//*******************************************************
static const DCI_RANGE_DATA_INDEX_TD DCI_FW_UPGRADE_MODE_RANGE_INDEX = 0U;
static const DCI_RANGE_DATA_INDEX_TD DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_RANGE_INDEX = 1U;
static const DCI_RANGE_DATA_INDEX_TD DCI_IP_ADDRESS_ALLOCATION_METHOD_RANGE_INDEX = 2U;
static const DCI_RANGE_DATA_INDEX_TD DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_RANGE_INDEX = 3U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY1_LINK_SPEED_SELECT_RANGE_INDEX = 4U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY1_FULL_DUPLEX_ENABLE_RANGE_INDEX = 5U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY1_FULL_DUPLEX_ACTUAL_RANGE_INDEX = 6U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY1_AUTONEG_ENABLE_RANGE_INDEX = 7U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY1_PORT_ENABLE_RANGE_INDEX = 8U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY2_LINK_SPEED_SELECT_RANGE_INDEX = 9U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY2_FULL_DUPLEX_ENABLE_RANGE_INDEX = 10U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY2_FULL_DUPLEX_ACTUAL_RANGE_INDEX = 11U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY2_AUTONEG_ENABLE_RANGE_INDEX = 12U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PHY2_PORT_ENABLE_RANGE_INDEX = 13U;
static const DCI_RANGE_DATA_INDEX_TD DCI_ETH_ACD_ENABLE_RANGE_INDEX = 14U;
static const DCI_RANGE_DATA_INDEX_TD DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_RANGE_INDEX = 15U;
static const DCI_RANGE_DATA_INDEX_TD DCI_EXAMPLE_PWM_DUTYCYCLE_RANGE_INDEX = 16U;
static const DCI_RANGE_DATA_INDEX_TD DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_RANGE_INDEX = 17U;
static const DCI_RANGE_DATA_INDEX_TD DCI_EXAMPLE_MODBUS_SERIAL_PARITY_RANGE_INDEX = 18U;
static const DCI_RANGE_DATA_INDEX_TD DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_RANGE_INDEX = 19U;
static const DCI_RANGE_DATA_INDEX_TD DCI_HTTP_ENABLE_RANGE_INDEX = 20U;
static const DCI_RANGE_DATA_INDEX_TD DCI_IP_BIT_NOT_CONNECTED_RANGE_INDEX = 21U;
static const DCI_RANGE_DATA_INDEX_TD DCI_OP_BIT_NOT_CONNECTED_RANGE_INDEX = 22U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DEVICE_PARAMETERIZATION_ENABLE_RANGE_INDEX = 23U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PRODUCT_ACTIVE_FAULT_RANGE_INDEX = 24U;
static const DCI_RANGE_DATA_INDEX_TD DCI_SAMPLE_APP_OUTPUT_WORD_RANGE_INDEX = 25U;
static const DCI_RANGE_DATA_INDEX_TD DCI_SAMPLE_APP_OUTPUT_DWORD_RANGE_INDEX = 26U;
static const DCI_RANGE_DATA_INDEX_TD DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_RANGE_INDEX = 27U;
static const DCI_RANGE_DATA_INDEX_TD DCI_SAMPLE_APP_PARAMETER_RANGE_INDEX = 28U;
static const DCI_RANGE_DATA_INDEX_TD DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_RANGE_INDEX = 29U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_IN_ASM_SELECT_1_RANGE_INDEX = 30U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_IN_ASM_SELECT_2_RANGE_INDEX = 31U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_IN_ASM_SELECT_3_RANGE_INDEX = 32U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_IN_ASM_SELECT_4_RANGE_INDEX = 33U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_IN_ASM_SELECT_5_RANGE_INDEX = 34U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_IN_ASM_SELECT_6_RANGE_INDEX = 35U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_IN_ASM_SELECT_7_RANGE_INDEX = 36U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_IN_ASM_SELECT_8_RANGE_INDEX = 37U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_OUT_ASM_SELECT_1_RANGE_INDEX = 38U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_OUT_ASM_SELECT_2_RANGE_INDEX = 39U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_OUT_ASM_SELECT_3_RANGE_INDEX = 40U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DYN_OUT_ASM_SELECT_4_RANGE_INDEX = 41U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_RANGE_INDEX = 42U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_IP_UPD_PORT_NUM_RANGE_INDEX = 43U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_IP_FOREIGN_DEVICE_RANGE_INDEX = 44U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_IP_BBMD_PORT_RANGE_INDEX = 45U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_IP_COMM_TIMEOUT_RANGE_INDEX = 46U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_IP_PROTOCOL_STATUS_RANGE_INDEX = 47U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_IP_FAULT_BEHAVIOR_RANGE_INDEX = 48U;
static const DCI_RANGE_DATA_INDEX_TD DCI_ENABLE_BACNET_IP_RANGE_INDEX = 49U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_DEVICE_ADDR_RANGE_INDEX = 50U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_IP_INSTANCE_NUM_RANGE_INDEX = 51U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_MSTP_INSTANCE_NUM_RANGE_INDEX = 52U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_MSTP_COMM_TIMEOUT_RANGE_INDEX = 53U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_MSTP_BAUD_RANGE_INDEX = 54U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_MSTP_PARITY_RANGE_INDEX = 55U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_MSTP_STOPBIT_RANGE_INDEX = 56U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_MSTP_PROTOCOL_STATUS_RANGE_INDEX = 57U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_FAULT_CODE_RANGE_INDEX = 58U;
static const DCI_RANGE_DATA_INDEX_TD DCI_TIME_OFFSET_RANGE_INDEX = 59U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DATE_FORMAT_RANGE_INDEX = 60U;
static const DCI_RANGE_DATA_INDEX_TD DCI_TIME_FORMAT_RANGE_INDEX = 61U;
static const DCI_RANGE_DATA_INDEX_TD DCI_REST_RESET_COMMAND_RANGE_INDEX = 62U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DEVICE_CERT_VALID_RANGE_INDEX = 63U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DEVICE_CERT_CONTROL_RANGE_INDEX = 64U;
static const DCI_RANGE_DATA_INDEX_TD DCI_SNTP_SERVICE_ENABLE_RANGE_INDEX = 65U;
static const DCI_RANGE_DATA_INDEX_TD DCI_SNTP_UPDATE_TIME_RANGE_INDEX = 66U;
static const DCI_RANGE_DATA_INDEX_TD DCI_SNTP_RETRY_TIME_RANGE_INDEX = 67U;
static const DCI_RANGE_DATA_INDEX_TD DCI_SNTP_ACTIVE_SERVER_RANGE_INDEX = 68U;
static const DCI_RANGE_DATA_INDEX_TD DCI_IOT_UPDATE_RATE_RANGE_INDEX = 69U;
static const DCI_RANGE_DATA_INDEX_TD DCI_IOT_CADENCE_UPDATE_RATE_RANGE_INDEX = 70U;
static const DCI_RANGE_DATA_INDEX_TD DCI_GROUP0_CADENCE_RANGE_INDEX = 71U;
static const DCI_RANGE_DATA_INDEX_TD DCI_GROUP1_CADENCE_RANGE_INDEX = 72U;
static const DCI_RANGE_DATA_INDEX_TD DCI_GROUP2_CADENCE_RANGE_INDEX = 73U;
static const DCI_RANGE_DATA_INDEX_TD DCI_GROUP3_CADENCE_RANGE_INDEX = 74U;
static const DCI_RANGE_DATA_INDEX_TD DCI_IOT_CONN_STAT_REASON_RANGE_INDEX = 75U;
static const DCI_RANGE_DATA_INDEX_TD DCI_CHANGE_BASED_LOGGING_RANGE_INDEX = 76U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_MIN_VAL_CONN_INTERVAL_RANGE_INDEX = 77U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_MAX_VAL_CONN_INTERVAL_RANGE_INDEX = 78U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_SLAVE_LATENCY_RANGE_INDEX = 79U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_SUPERVISION_TIMEOUT_RANGE_INDEX = 80U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_SETTING_2_RANGE_INDEX = 81U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_SETTING_C_RANGE_INDEX = 82U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_SETTING_E_RANGE_INDEX = 83U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_SETTING_F_RANGE_INDEX = 84U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_READING_2_RANGE_INDEX = 85U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_READING_3_RANGE_INDEX = 86U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_SETTING_5_RANGE_INDEX = 87U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_SETTING_6_RANGE_INDEX = 88U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_SETTING_7_RANGE_INDEX = 89U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_PRESENT_STATUS_RANGE_INDEX = 90U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_TEST_EVENT_LIST_RANGE_INDEX = 91U;
static const DCI_RANGE_DATA_INDEX_TD DCI_LOGGING_EVENT_CODE_RANGE_INDEX = 92U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DEVICE_STATE_RANGE_INDEX = 93U;
static const DCI_RANGE_DATA_INDEX_TD DCI_NV_MEM_ID_RANGE_INDEX = 94U;
static const DCI_RANGE_DATA_INDEX_TD DCI_NV_FAIL_OPERATION_RANGE_INDEX = 95U;
static const DCI_RANGE_DATA_INDEX_TD DCI_LOG_PROC_IMAGE_ID_RANGE_INDEX = 96U;
static const DCI_RANGE_DATA_INDEX_TD DCI_MODBUS_TCP_ENABLE_RANGE_INDEX = 97U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BACNET_IP_ENABLE_RANGE_INDEX = 98U;
static const DCI_RANGE_DATA_INDEX_TD DCI_LOG_PORT_NUMBER_RANGE_INDEX = 99U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_USER1_ROLE_RANGE_INDEX = 100U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_USER2_ROLE_RANGE_INDEX = 101U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_USER3_ROLE_RANGE_INDEX = 102U;
static const DCI_RANGE_DATA_INDEX_TD DCI_BLE_USER4_ROLE_RANGE_INDEX = 103U;
static const DCI_RANGE_DATA_INDEX_TD DCI_RESET_SOURCE_RANGE_INDEX = 104U;
static const DCI_RANGE_DATA_INDEX_TD DCI_MAX_CONCURRENT_SESSION_RANGE_INDEX = 105U;
static const DCI_RANGE_DATA_INDEX_TD DCI_USER_INACTIVITY_TIMEOUT_RANGE_INDEX = 106U;
static const DCI_RANGE_DATA_INDEX_TD DCI_ABSOLUTE_TIMEOUT_SEC_RANGE_INDEX = 107U;
static const DCI_RANGE_DATA_INDEX_TD DCI_USER_LOCK_TIME_SEC_RANGE_INDEX = 108U;
static const DCI_RANGE_DATA_INDEX_TD DCI_MAX_FAILED_LOGIN_ATTEMPTS_RANGE_INDEX = 109U;
static const DCI_RANGE_DATA_INDEX_TD DCI_CORS_TYPE_RANGE_INDEX = 110U;
static const DCI_RANGE_DATA_INDEX_TD DCI_DEVICE_PWD_SETTING_RANGE_INDEX = 111U;
static const DCI_RANGE_DATA_INDEX_TD DCI_LANG_PREF_SETTING_RANGE_INDEX = 112U;
static const DCI_RANGE_DATA_INDEX_TD DCI_PTP_ENABLE_RANGE_INDEX = 113U;
static const DCI_RANGE_DATA_INDEX_TD DCI_CLIENT_AP_SECURITY_RANGE_INDEX = 114U;
static const DCI_RANGE_DATA_INDEX_TD DCI_CLIENT_AP_BAND_RANGE_INDEX = 115U;


//*******************************************************
//******     The Enum count
//*******************************************************
static const DCI_ENUM_COUNT_TD DCI_FW_UPGRADE_MODE_ENUM_COUNT = 3U;
static const DCI_ENUM_COUNT_TD DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_PHY1_LINK_SPEED_SELECT_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_PHY2_LINK_SPEED_SELECT_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_ENUM_COUNT = 5U;
static const DCI_ENUM_COUNT_TD DCI_EXAMPLE_MODBUS_SERIAL_PARITY_ENUM_COUNT = 3U;
static const DCI_ENUM_COUNT_TD DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_HTTP_ENABLE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_DEVICE_PARAMETERIZATION_ENABLE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_PRODUCT_ACTIVE_FAULT_ENUM_COUNT = 49U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_IP_UPD_PORT_NUM_ENUM_COUNT = 16U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_IP_FOREIGN_DEVICE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_IP_BBMD_PORT_ENUM_COUNT = 16U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_IP_PROTOCOL_STATUS_ENUM_COUNT = 3U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_IP_FAULT_BEHAVIOR_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_ENABLE_BACNET_IP_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_MSTP_BAUD_ENUM_COUNT = 5U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_MSTP_PARITY_ENUM_COUNT = 3U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_MSTP_STOPBIT_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_MSTP_PROTOCOL_STATUS_ENUM_COUNT = 3U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_FAULT_CODE_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_TIME_OFFSET_ENUM_COUNT = 32U;
static const DCI_ENUM_COUNT_TD DCI_DATE_FORMAT_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_TIME_FORMAT_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_REST_RESET_COMMAND_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_DEVICE_CERT_VALID_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_DEVICE_CERT_CONTROL_ENUM_COUNT = 3U;
static const DCI_ENUM_COUNT_TD DCI_SNTP_SERVICE_ENABLE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_SNTP_ACTIVE_SERVER_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_IOT_UPDATE_RATE_ENUM_COUNT = 6U;
static const DCI_ENUM_COUNT_TD DCI_IOT_CADENCE_UPDATE_RATE_ENUM_COUNT = 6U;
static const DCI_ENUM_COUNT_TD DCI_GROUP0_CADENCE_ENUM_COUNT = 7U;
static const DCI_ENUM_COUNT_TD DCI_GROUP1_CADENCE_ENUM_COUNT = 7U;
static const DCI_ENUM_COUNT_TD DCI_GROUP2_CADENCE_ENUM_COUNT = 7U;
static const DCI_ENUM_COUNT_TD DCI_GROUP3_CADENCE_ENUM_COUNT = 7U;
static const DCI_ENUM_COUNT_TD DCI_IOT_CONN_STAT_REASON_ENUM_COUNT = 18U;
static const DCI_ENUM_COUNT_TD DCI_CHANGE_BASED_LOGGING_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_BLE_TEST_SETTING_F_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_LOGGING_EVENT_CODE_ENUM_COUNT = 38U;
static const DCI_ENUM_COUNT_TD DCI_DEVICE_STATE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_NV_MEM_ID_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_NV_FAIL_OPERATION_ENUM_COUNT = 6U;
static const DCI_ENUM_COUNT_TD DCI_LOG_PROC_IMAGE_ID_ENUM_COUNT = 3U;
static const DCI_ENUM_COUNT_TD DCI_MODBUS_TCP_ENABLE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_BACNET_IP_ENABLE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_LOG_PORT_NUMBER_ENUM_COUNT = 8U;
static const DCI_ENUM_COUNT_TD DCI_BLE_USER1_ROLE_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_BLE_USER2_ROLE_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_BLE_USER3_ROLE_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_BLE_USER4_ROLE_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_RESET_SOURCE_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_CORS_TYPE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_DEVICE_PWD_SETTING_ENUM_COUNT = 3U;
static const DCI_ENUM_COUNT_TD DCI_LANG_PREF_SETTING_ENUM_COUNT = 4U;
static const DCI_ENUM_COUNT_TD DCI_PTP_ENABLE_ENUM_COUNT = 2U;
static const DCI_ENUM_COUNT_TD DCI_CLIENT_AP_SECURITY_ENUM_COUNT = 21U;
static const DCI_ENUM_COUNT_TD DCI_CLIENT_AP_BAND_ENUM_COUNT = 2U;


//*******************************************************
//******     The Enum index
//*******************************************************
static const DCI_ENUM_INDEX_TD DCI_FW_UPGRADE_MODE_ENUM_INDEX = 0U;
static const DCI_ENUM_INDEX_TD DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_ENUM_INDEX = 1U;
static const DCI_ENUM_INDEX_TD DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM_INDEX = 2U;
static const DCI_ENUM_INDEX_TD DCI_PHY1_LINK_SPEED_SELECT_ENUM_INDEX = 3U;
static const DCI_ENUM_INDEX_TD DCI_PHY2_LINK_SPEED_SELECT_ENUM_INDEX = 4U;
static const DCI_ENUM_INDEX_TD DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_ENUM_INDEX = 5U;
static const DCI_ENUM_INDEX_TD DCI_EXAMPLE_MODBUS_SERIAL_PARITY_ENUM_INDEX = 6U;
static const DCI_ENUM_INDEX_TD DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_ENUM_INDEX = 7U;
static const DCI_ENUM_INDEX_TD DCI_HTTP_ENABLE_ENUM_INDEX = 8U;
static const DCI_ENUM_INDEX_TD DCI_DEVICE_PARAMETERIZATION_ENABLE_ENUM_INDEX = 9U;
static const DCI_ENUM_INDEX_TD DCI_PRODUCT_ACTIVE_FAULT_ENUM_INDEX = 10U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_IP_UPD_PORT_NUM_ENUM_INDEX = 11U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_IP_FOREIGN_DEVICE_ENUM_INDEX = 12U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_IP_BBMD_PORT_ENUM_INDEX = 13U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_IP_PROTOCOL_STATUS_ENUM_INDEX = 14U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_IP_FAULT_BEHAVIOR_ENUM_INDEX = 15U;
static const DCI_ENUM_INDEX_TD DCI_ENABLE_BACNET_IP_ENUM_INDEX = 16U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_MSTP_BAUD_ENUM_INDEX = 17U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_MSTP_PARITY_ENUM_INDEX = 18U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_MSTP_STOPBIT_ENUM_INDEX = 19U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_MSTP_PROTOCOL_STATUS_ENUM_INDEX = 20U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_FAULT_CODE_ENUM_INDEX = 21U;
static const DCI_ENUM_INDEX_TD DCI_TIME_OFFSET_ENUM_INDEX = 22U;
static const DCI_ENUM_INDEX_TD DCI_DATE_FORMAT_ENUM_INDEX = 23U;
static const DCI_ENUM_INDEX_TD DCI_TIME_FORMAT_ENUM_INDEX = 24U;
static const DCI_ENUM_INDEX_TD DCI_REST_RESET_COMMAND_ENUM_INDEX = 25U;
static const DCI_ENUM_INDEX_TD DCI_DEVICE_CERT_VALID_ENUM_INDEX = 26U;
static const DCI_ENUM_INDEX_TD DCI_DEVICE_CERT_CONTROL_ENUM_INDEX = 27U;
static const DCI_ENUM_INDEX_TD DCI_SNTP_SERVICE_ENABLE_ENUM_INDEX = 28U;
static const DCI_ENUM_INDEX_TD DCI_SNTP_ACTIVE_SERVER_ENUM_INDEX = 29U;
static const DCI_ENUM_INDEX_TD DCI_IOT_UPDATE_RATE_ENUM_INDEX = 30U;
static const DCI_ENUM_INDEX_TD DCI_IOT_CADENCE_UPDATE_RATE_ENUM_INDEX = 31U;
static const DCI_ENUM_INDEX_TD DCI_GROUP0_CADENCE_ENUM_INDEX = 32U;
static const DCI_ENUM_INDEX_TD DCI_GROUP1_CADENCE_ENUM_INDEX = 33U;
static const DCI_ENUM_INDEX_TD DCI_GROUP2_CADENCE_ENUM_INDEX = 34U;
static const DCI_ENUM_INDEX_TD DCI_GROUP3_CADENCE_ENUM_INDEX = 35U;
static const DCI_ENUM_INDEX_TD DCI_IOT_CONN_STAT_REASON_ENUM_INDEX = 36U;
static const DCI_ENUM_INDEX_TD DCI_CHANGE_BASED_LOGGING_ENUM_INDEX = 37U;
static const DCI_ENUM_INDEX_TD DCI_BLE_TEST_SETTING_F_ENUM_INDEX = 38U;
static const DCI_ENUM_INDEX_TD DCI_LOGGING_EVENT_CODE_ENUM_INDEX = 39U;
static const DCI_ENUM_INDEX_TD DCI_DEVICE_STATE_ENUM_INDEX = 40U;
static const DCI_ENUM_INDEX_TD DCI_NV_MEM_ID_ENUM_INDEX = 41U;
static const DCI_ENUM_INDEX_TD DCI_NV_FAIL_OPERATION_ENUM_INDEX = 42U;
static const DCI_ENUM_INDEX_TD DCI_LOG_PROC_IMAGE_ID_ENUM_INDEX = 43U;
static const DCI_ENUM_INDEX_TD DCI_MODBUS_TCP_ENABLE_ENUM_INDEX = 44U;
static const DCI_ENUM_INDEX_TD DCI_BACNET_IP_ENABLE_ENUM_INDEX = 45U;
static const DCI_ENUM_INDEX_TD DCI_LOG_PORT_NUMBER_ENUM_INDEX = 46U;
static const DCI_ENUM_INDEX_TD DCI_BLE_USER1_ROLE_ENUM_INDEX = 47U;
static const DCI_ENUM_INDEX_TD DCI_BLE_USER2_ROLE_ENUM_INDEX = 48U;
static const DCI_ENUM_INDEX_TD DCI_BLE_USER3_ROLE_ENUM_INDEX = 49U;
static const DCI_ENUM_INDEX_TD DCI_BLE_USER4_ROLE_ENUM_INDEX = 50U;
static const DCI_ENUM_INDEX_TD DCI_RESET_SOURCE_ENUM_INDEX = 51U;
static const DCI_ENUM_INDEX_TD DCI_CORS_TYPE_ENUM_INDEX = 52U;
static const DCI_ENUM_INDEX_TD DCI_DEVICE_PWD_SETTING_ENUM_INDEX = 53U;
static const DCI_ENUM_INDEX_TD DCI_LANG_PREF_SETTING_ENUM_INDEX = 54U;
static const DCI_ENUM_INDEX_TD DCI_PTP_ENABLE_ENUM_INDEX = 55U;
static const DCI_ENUM_INDEX_TD DCI_CLIENT_AP_SECURITY_ENUM_INDEX = 56U;
static const DCI_ENUM_INDEX_TD DCI_CLIENT_AP_BAND_ENUM_INDEX = 57U;


//*******************************************************
//******     The Enum count array
//*******************************************************
const DCI_ENUM_COUNT_TD  DCI_ENUM_COUNT_ARRAY[TOTAL_ENUM_COUNT_ARRAY_SIZE] = {
     DCI_FW_UPGRADE_MODE_ENUM_COUNT,
     DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_ENUM_COUNT,
     DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM_COUNT,
     DCI_PHY1_LINK_SPEED_SELECT_ENUM_COUNT,
     DCI_PHY2_LINK_SPEED_SELECT_ENUM_COUNT,
     DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_ENUM_COUNT,
     DCI_EXAMPLE_MODBUS_SERIAL_PARITY_ENUM_COUNT,
     DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_ENUM_COUNT,
     DCI_HTTP_ENABLE_ENUM_COUNT,
     DCI_DEVICE_PARAMETERIZATION_ENABLE_ENUM_COUNT,
     DCI_PRODUCT_ACTIVE_FAULT_ENUM_COUNT,
     DCI_BACNET_IP_UPD_PORT_NUM_ENUM_COUNT,
     DCI_BACNET_IP_FOREIGN_DEVICE_ENUM_COUNT,
     DCI_BACNET_IP_BBMD_PORT_ENUM_COUNT,
     DCI_BACNET_IP_PROTOCOL_STATUS_ENUM_COUNT,
     DCI_BACNET_IP_FAULT_BEHAVIOR_ENUM_COUNT,
     DCI_ENABLE_BACNET_IP_ENUM_COUNT,
     DCI_BACNET_MSTP_BAUD_ENUM_COUNT,
     DCI_BACNET_MSTP_PARITY_ENUM_COUNT,
     DCI_BACNET_MSTP_STOPBIT_ENUM_COUNT,
     DCI_BACNET_MSTP_PROTOCOL_STATUS_ENUM_COUNT,
     DCI_BACNET_FAULT_CODE_ENUM_COUNT,
     DCI_TIME_OFFSET_ENUM_COUNT,
     DCI_DATE_FORMAT_ENUM_COUNT,
     DCI_TIME_FORMAT_ENUM_COUNT,
     DCI_REST_RESET_COMMAND_ENUM_COUNT,
     DCI_DEVICE_CERT_VALID_ENUM_COUNT,
     DCI_DEVICE_CERT_CONTROL_ENUM_COUNT,
     DCI_SNTP_SERVICE_ENABLE_ENUM_COUNT,
     DCI_SNTP_ACTIVE_SERVER_ENUM_COUNT,
     DCI_IOT_UPDATE_RATE_ENUM_COUNT,
     DCI_IOT_CADENCE_UPDATE_RATE_ENUM_COUNT,
     DCI_GROUP0_CADENCE_ENUM_COUNT,
     DCI_GROUP1_CADENCE_ENUM_COUNT,
     DCI_GROUP2_CADENCE_ENUM_COUNT,
     DCI_GROUP3_CADENCE_ENUM_COUNT,
     DCI_IOT_CONN_STAT_REASON_ENUM_COUNT,
     DCI_CHANGE_BASED_LOGGING_ENUM_COUNT,
     DCI_BLE_TEST_SETTING_F_ENUM_COUNT,
     DCI_LOGGING_EVENT_CODE_ENUM_COUNT,
     DCI_DEVICE_STATE_ENUM_COUNT,
     DCI_NV_MEM_ID_ENUM_COUNT,
     DCI_NV_FAIL_OPERATION_ENUM_COUNT,
     DCI_LOG_PROC_IMAGE_ID_ENUM_COUNT,
     DCI_MODBUS_TCP_ENABLE_ENUM_COUNT,
     DCI_BACNET_IP_ENABLE_ENUM_COUNT,
     DCI_LOG_PORT_NUMBER_ENUM_COUNT,
     DCI_BLE_USER1_ROLE_ENUM_COUNT,
     DCI_BLE_USER2_ROLE_ENUM_COUNT,
     DCI_BLE_USER3_ROLE_ENUM_COUNT,
     DCI_BLE_USER4_ROLE_ENUM_COUNT,
     DCI_RESET_SOURCE_ENUM_COUNT,
     DCI_CORS_TYPE_ENUM_COUNT,
     DCI_DEVICE_PWD_SETTING_ENUM_COUNT,
     DCI_LANG_PREF_SETTING_ENUM_COUNT,
     DCI_PTP_ENABLE_ENUM_COUNT,
     DCI_CLIENT_AP_SECURITY_ENUM_COUNT,
     DCI_CLIENT_AP_BAND_ENUM_COUNT
};


//*******************************************************
//******     The Enum list
//*******************************************************
static const uint32_t  DCI_BACNET_MSTP_BAUD_ENUM_LIST[DCI_BACNET_MSTP_BAUD_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U, 4U  };
static const uint32_t  DCI_RESET_SOURCE_ENUM_LIST[DCI_RESET_SOURCE_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const int32_t  DCI_GROUP0_CADENCE_ENUM_LIST[DCI_GROUP0_CADENCE_ENUM_COUNT] = 
    {  -1, 0, 1000, 5000, 10000, 15000, 20000  };
static const int32_t  DCI_GROUP1_CADENCE_ENUM_LIST[DCI_GROUP1_CADENCE_ENUM_COUNT] = 
    {  -1, 10000, 60000, 120000, 300000, 500000, 600000  };
static const int32_t  DCI_GROUP2_CADENCE_ENUM_LIST[DCI_GROUP2_CADENCE_ENUM_COUNT] = 
    {  -1, 0, 1000, 5000, 10000, 15000, 20000  };
static const int32_t  DCI_GROUP3_CADENCE_ENUM_LIST[DCI_GROUP3_CADENCE_ENUM_COUNT] = 
    {  -1, 0, 1000, 5000, 10000, 15000, 20000  };
static const int32_t  DCI_CLIENT_AP_SECURITY_ENUM_LIST[DCI_CLIENT_AP_SECURITY_ENUM_COUNT] = 
    {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20  };
static const uint16_t  DCI_PHY1_LINK_SPEED_SELECT_ENUM_LIST[DCI_PHY1_LINK_SPEED_SELECT_ENUM_COUNT] = 
    {  10U, 100U  };
static const uint16_t  DCI_PHY2_LINK_SPEED_SELECT_ENUM_LIST[DCI_PHY2_LINK_SPEED_SELECT_ENUM_COUNT] = 
    {  10U, 100U  };
static const uint16_t  DCI_EXAMPLE_MODBUS_SERIAL_PARITY_ENUM_LIST[DCI_EXAMPLE_MODBUS_SERIAL_PARITY_ENUM_COUNT] = 
    {  0U, 1U, 2U  };
static const uint16_t  DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_ENUM_LIST[DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint16_t  DCI_PRODUCT_ACTIVE_FAULT_ENUM_LIST[DCI_PRODUCT_ACTIVE_FAULT_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 37U, 500U, 501U, 502U, 503U, 504U, 505U, 506U, 507U, 508U, 509U, 510U  };
static const uint16_t  DCI_BACNET_IP_UPD_PORT_NUM_ENUM_LIST[DCI_BACNET_IP_UPD_PORT_NUM_ENUM_COUNT] = 
    {  47808U, 47809U, 47810U, 47811U, 47812U, 47813U, 47814U, 47815U, 47816U, 47817U, 47818U, 47819U, 47820U, 47821U, 47822U, 47823U  };
static const uint16_t  DCI_BACNET_IP_BBMD_PORT_ENUM_LIST[DCI_BACNET_IP_BBMD_PORT_ENUM_COUNT] = 
    {  47808U, 47809U, 47810U, 47811U, 47812U, 47813U, 47814U, 47815U, 47816U, 47817U, 47818U, 47819U, 47820U, 47821U, 47822U, 47823U  };
static const uint16_t  DCI_BACNET_MSTP_PARITY_ENUM_LIST[DCI_BACNET_MSTP_PARITY_ENUM_COUNT] = 
    {  0U, 1U, 2U  };
static const uint16_t  DCI_BACNET_MSTP_STOPBIT_ENUM_LIST[DCI_BACNET_MSTP_STOPBIT_ENUM_COUNT] = 
    {  0U, 1U  };
static const int16_t  DCI_TIME_OFFSET_ENUM_LIST[DCI_TIME_OFFSET_ENUM_COUNT] = 
    {  660, 600, 540, 480, 420, 360, 300, 270, 240, 210, 180, 120, 60, 0, -60, -120, -180, -210, -240, -300, -330, -345, -360, -390, -420, -480, -540, -570, -600, -660, -720, -780  };
static const uint8_t  DCI_FW_UPGRADE_MODE_ENUM_LIST[DCI_FW_UPGRADE_MODE_ENUM_COUNT] = 
    {  0U, 1U, 2U  };
static const uint8_t  DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_ENUM_LIST[DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_ENUM_COUNT] = 
    {  1U, 2U  };
static const uint8_t  DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM_LIST[DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_ENUM_LIST[DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_ENUM_COUNT] = 
    {  0U, 7U, 15U, 31U, 99U  };
static const uint8_t  DCI_BACNET_IP_FOREIGN_DEVICE_ENUM_LIST[DCI_BACNET_IP_FOREIGN_DEVICE_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_BACNET_IP_PROTOCOL_STATUS_ENUM_LIST[DCI_BACNET_IP_PROTOCOL_STATUS_ENUM_COUNT] = 
    {  0U, 1U, 2U  };
static const uint8_t  DCI_BACNET_IP_FAULT_BEHAVIOR_ENUM_LIST[DCI_BACNET_IP_FAULT_BEHAVIOR_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_ENABLE_BACNET_IP_ENUM_LIST[DCI_ENABLE_BACNET_IP_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_BACNET_MSTP_PROTOCOL_STATUS_ENUM_LIST[DCI_BACNET_MSTP_PROTOCOL_STATUS_ENUM_COUNT] = 
    {  0U, 1U, 2U  };
static const uint8_t  DCI_BACNET_FAULT_CODE_ENUM_LIST[DCI_BACNET_FAULT_CODE_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_DATE_FORMAT_ENUM_LIST[DCI_DATE_FORMAT_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_TIME_FORMAT_ENUM_LIST[DCI_TIME_FORMAT_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_REST_RESET_COMMAND_ENUM_LIST[DCI_REST_RESET_COMMAND_ENUM_COUNT] = 
    {  1U, 2U  };
static const uint8_t  DCI_DEVICE_CERT_VALID_ENUM_LIST[DCI_DEVICE_CERT_VALID_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_DEVICE_CERT_CONTROL_ENUM_LIST[DCI_DEVICE_CERT_CONTROL_ENUM_COUNT] = 
    {  0U, 1U, 2U  };
static const uint8_t  DCI_SNTP_SERVICE_ENABLE_ENUM_LIST[DCI_SNTP_SERVICE_ENABLE_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_SNTP_ACTIVE_SERVER_ENUM_LIST[DCI_SNTP_ACTIVE_SERVER_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_IOT_UPDATE_RATE_ENUM_LIST[DCI_IOT_UPDATE_RATE_ENUM_COUNT] = 
    {  5U, 10U, 15U, 20U, 25U, 30U  };
static const uint8_t  DCI_IOT_CADENCE_UPDATE_RATE_ENUM_LIST[DCI_IOT_CADENCE_UPDATE_RATE_ENUM_COUNT] = 
    {  5U, 10U, 15U, 20U, 25U, 30U  };
static const uint8_t  DCI_IOT_CONN_STAT_REASON_ENUM_LIST[DCI_IOT_CONN_STAT_REASON_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U  };
static const uint8_t  DCI_CHANGE_BASED_LOGGING_ENUM_LIST[DCI_CHANGE_BASED_LOGGING_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_BLE_TEST_SETTING_F_ENUM_LIST[DCI_BLE_TEST_SETTING_F_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_LOGGING_EVENT_CODE_ENUM_LIST[DCI_LOGGING_EVENT_CODE_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 26U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 101U, 102U  };
static const uint8_t  DCI_DEVICE_STATE_ENUM_LIST[DCI_DEVICE_STATE_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_NV_MEM_ID_ENUM_LIST[DCI_NV_MEM_ID_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_NV_FAIL_OPERATION_ENUM_LIST[DCI_NV_FAIL_OPERATION_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U, 4U, 5U  };
static const uint8_t  DCI_LOG_PROC_IMAGE_ID_ENUM_LIST[DCI_LOG_PROC_IMAGE_ID_ENUM_COUNT] = 
    {  0U, 1U, 2U  };
static const uint8_t  DCI_LOG_PORT_NUMBER_ENUM_LIST[DCI_LOG_PORT_NUMBER_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U  };
static const uint8_t  DCI_BLE_USER1_ROLE_ENUM_LIST[DCI_BLE_USER1_ROLE_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_BLE_USER2_ROLE_ENUM_LIST[DCI_BLE_USER2_ROLE_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_BLE_USER3_ROLE_ENUM_LIST[DCI_BLE_USER3_ROLE_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_BLE_USER4_ROLE_ENUM_LIST[DCI_BLE_USER4_ROLE_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_CORS_TYPE_ENUM_LIST[DCI_CORS_TYPE_ENUM_COUNT] = 
    {  0U, 1U  };
static const uint8_t  DCI_DEVICE_PWD_SETTING_ENUM_LIST[DCI_DEVICE_PWD_SETTING_ENUM_COUNT] = 
    {  0U, 1U, 2U  };
static const uint8_t  DCI_LANG_PREF_SETTING_ENUM_LIST[DCI_LANG_PREF_SETTING_ENUM_COUNT] = 
    {  0U, 1U, 2U, 3U  };
static const uint8_t  DCI_CLIENT_AP_BAND_ENUM_LIST[DCI_CLIENT_AP_BAND_ENUM_COUNT] = 
    {  0U, 1U  };
static const dci_bool_t  DCI_HTTP_ENABLE_ENUM_LIST[DCI_HTTP_ENABLE_ENUM_COUNT] = 
    {  0U, 1U  };
static const dci_bool_t  DCI_DEVICE_PARAMETERIZATION_ENABLE_ENUM_LIST[DCI_DEVICE_PARAMETERIZATION_ENABLE_ENUM_COUNT] = 
    {  0U, 1U  };
static const dci_bool_t  DCI_MODBUS_TCP_ENABLE_ENUM_LIST[DCI_MODBUS_TCP_ENABLE_ENUM_COUNT] = 
    {  0U, 1U  };
static const dci_bool_t  DCI_BACNET_IP_ENABLE_ENUM_LIST[DCI_BACNET_IP_ENABLE_ENUM_COUNT] = 
    {  0U, 1U  };
static const dci_bool_t  DCI_PTP_ENABLE_ENUM_LIST[DCI_PTP_ENABLE_ENUM_COUNT] = 
    {  0U, 1U  };


//*******************************************************
//******     The Range/Enum Block
//*******************************************************
const DCI_ENUM_BLOCK  DCI_ENUM_BLOCK_ARRAY[TOTAL_ENUM_COUNT_ARRAY_SIZE] = {
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_FW_UPGRADE_MODE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_PHY1_LINK_SPEED_SELECT_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_PHY2_LINK_SPEED_SELECT_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_EXAMPLE_MODBUS_SERIAL_PARITY_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_HTTP_ENABLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_DEVICE_PARAMETERIZATION_ENABLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_PRODUCT_ACTIVE_FAULT_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_IP_UPD_PORT_NUM_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_IP_FOREIGN_DEVICE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_IP_BBMD_PORT_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_IP_PROTOCOL_STATUS_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_IP_FAULT_BEHAVIOR_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_ENABLE_BACNET_IP_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_MSTP_BAUD_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_MSTP_PARITY_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_MSTP_STOPBIT_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_MSTP_PROTOCOL_STATUS_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_FAULT_CODE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_TIME_OFFSET_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_DATE_FORMAT_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_TIME_FORMAT_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_REST_RESET_COMMAND_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_DEVICE_CERT_VALID_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_DEVICE_CERT_CONTROL_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_SNTP_SERVICE_ENABLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_SNTP_ACTIVE_SERVER_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_IOT_UPDATE_RATE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_IOT_CADENCE_UPDATE_RATE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_GROUP0_CADENCE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_GROUP1_CADENCE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_GROUP2_CADENCE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_GROUP3_CADENCE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_IOT_CONN_STAT_REASON_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_CHANGE_BASED_LOGGING_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BLE_TEST_SETTING_F_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_LOGGING_EVENT_CODE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_DEVICE_STATE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_NV_MEM_ID_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_NV_FAIL_OPERATION_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_LOG_PROC_IMAGE_ID_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_MODBUS_TCP_ENABLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BACNET_IP_ENABLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_LOG_PORT_NUMBER_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BLE_USER1_ROLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BLE_USER2_ROLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BLE_USER3_ROLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_BLE_USER4_ROLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_RESET_SOURCE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_CORS_TYPE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_DEVICE_PWD_SETTING_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_LANG_PREF_SETTING_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_PTP_ENABLE_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_CLIENT_AP_SECURITY_ENUM_LIST )
},
{
    reinterpret_cast<DCI_ENUM_LIST_TD const*>( DCI_CLIENT_AP_BAND_ENUM_LIST )
}
};


//*******************************************************
//******     The Range/Enum Block min and max attribute
//*******************************************************
static const float64_t  DCI_BLE_TEST_READING_3_MIN_ATTRIB = 500;
static const float64_t  DCI_BLE_TEST_READING_3_MAX_ATTRIB = 54235645;
static const uint64_t  DCI_BLE_TEST_SETTING_C_MIN_ATTRIB = 6000UL;
static const uint64_t  DCI_BLE_TEST_SETTING_C_MAX_ATTRIB = 54321456UL;
static const int64_t  DCI_BLE_TEST_EVENT_LIST_MIN_ATTRIB = -700;
static const int64_t  DCI_BLE_TEST_EVENT_LIST_MAX_ATTRIB = 54321456;
static const float32_t  DCI_BLE_TEST_READING_2_MIN_ATTRIB = 500.0F;
static const float32_t  DCI_BLE_TEST_READING_2_MAX_ATTRIB = 5423564.0F;
static const uint32_t  DCI_SAMPLE_APP_OUTPUT_DWORD_MIN_ATTRIB = 0U;
static const uint32_t  DCI_SAMPLE_APP_OUTPUT_DWORD_MAX_ATTRIB = 0xffffffe0U;
static const uint32_t  DCI_BACNET_IP_INSTANCE_NUM_MIN_ATTRIB = 0U;
static const uint32_t  DCI_BACNET_IP_INSTANCE_NUM_MAX_ATTRIB = 4194302U;
static const uint32_t  DCI_BACNET_MSTP_INSTANCE_NUM_MIN_ATTRIB = 0U;
static const uint32_t  DCI_BACNET_MSTP_INSTANCE_NUM_MAX_ATTRIB = 4194302U;
static const uint32_t  DCI_SNTP_UPDATE_TIME_MIN_ATTRIB = 60000U;
static const uint32_t  DCI_SNTP_UPDATE_TIME_MAX_ATTRIB = 86400000U;
static const uint32_t  DCI_BLE_TEST_SETTING_7_MIN_ATTRIB = 1000U;
static const uint32_t  DCI_BLE_TEST_SETTING_7_MAX_ATTRIB = 243214562U;
static const uint32_t  DCI_ABSOLUTE_TIMEOUT_SEC_MIN_ATTRIB = 1800U;
static const uint32_t  DCI_ABSOLUTE_TIMEOUT_SEC_MAX_ATTRIB = 43200U;
static const int32_t  DCI_BLE_TEST_SETTING_6_MIN_ATTRIB = -600;
static const int32_t  DCI_BLE_TEST_SETTING_6_MAX_ATTRIB = 60000;
static const uint16_t  DCI_SAMPLE_APP_OUTPUT_WORD_MIN_ATTRIB = 0U;
static const uint16_t  DCI_SAMPLE_APP_OUTPUT_WORD_MAX_ATTRIB = 0xffe0U;
static const uint16_t  DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_MIN_ATTRIB = 0U;
static const uint16_t  DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_MAX_ATTRIB = 3600U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_1_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_1_MAX_ATTRIB = 19U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_2_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_2_MAX_ATTRIB = 19U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_3_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_3_MAX_ATTRIB = 19U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_4_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_4_MAX_ATTRIB = 19U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_5_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_5_MAX_ATTRIB = 19U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_6_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_6_MAX_ATTRIB = 19U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_7_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_7_MAX_ATTRIB = 19U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_8_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_IN_ASM_SELECT_8_MAX_ATTRIB = 19U;
static const uint16_t  DCI_DYN_OUT_ASM_SELECT_1_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_OUT_ASM_SELECT_1_MAX_ATTRIB = 4U;
static const uint16_t  DCI_DYN_OUT_ASM_SELECT_2_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_OUT_ASM_SELECT_2_MAX_ATTRIB = 4U;
static const uint16_t  DCI_DYN_OUT_ASM_SELECT_3_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_OUT_ASM_SELECT_3_MAX_ATTRIB = 4U;
static const uint16_t  DCI_DYN_OUT_ASM_SELECT_4_MIN_ATTRIB = 1U;
static const uint16_t  DCI_DYN_OUT_ASM_SELECT_4_MAX_ATTRIB = 4U;
static const uint16_t  DCI_BACNET_IP_COMM_TIMEOUT_MIN_ATTRIB = 0U;
static const uint16_t  DCI_BACNET_IP_COMM_TIMEOUT_MAX_ATTRIB = 60000U;
static const uint16_t  DCI_BACNET_MSTP_COMM_TIMEOUT_MIN_ATTRIB = 0U;
static const uint16_t  DCI_BACNET_MSTP_COMM_TIMEOUT_MAX_ATTRIB = 60000U;
static const uint16_t  DCI_SNTP_RETRY_TIME_MIN_ATTRIB = 15000U;
static const uint16_t  DCI_SNTP_RETRY_TIME_MAX_ATTRIB = 20000U;
static const uint16_t  DCI_BLE_MIN_VAL_CONN_INTERVAL_MIN_ATTRIB = 6U;
static const uint16_t  DCI_BLE_MIN_VAL_CONN_INTERVAL_MAX_ATTRIB = 3200U;
static const uint16_t  DCI_BLE_MAX_VAL_CONN_INTERVAL_MIN_ATTRIB = 6U;
static const uint16_t  DCI_BLE_MAX_VAL_CONN_INTERVAL_MAX_ATTRIB = 3200U;
static const uint16_t  DCI_BLE_SLAVE_LATENCY_MIN_ATTRIB = 0U;
static const uint16_t  DCI_BLE_SLAVE_LATENCY_MAX_ATTRIB = 500U;
static const uint16_t  DCI_BLE_SUPERVISION_TIMEOUT_MIN_ATTRIB = 10U;
static const uint16_t  DCI_BLE_SUPERVISION_TIMEOUT_MAX_ATTRIB = 3200U;
static const uint16_t  DCI_BLE_TEST_SETTING_E_MIN_ATTRIB = 0U;
static const uint16_t  DCI_BLE_TEST_SETTING_E_MAX_ATTRIB = 20U;
static const uint16_t  DCI_BLE_TEST_PRESENT_STATUS_MIN_ATTRIB = 500U;
static const uint16_t  DCI_BLE_TEST_PRESENT_STATUS_MAX_ATTRIB = 65535U;
static const uint16_t  DCI_USER_INACTIVITY_TIMEOUT_MIN_ATTRIB = 900U;
static const uint16_t  DCI_USER_INACTIVITY_TIMEOUT_MAX_ATTRIB = 1800U;
static const uint16_t  DCI_USER_LOCK_TIME_SEC_MIN_ATTRIB = 300U;
static const uint16_t  DCI_USER_LOCK_TIME_SEC_MAX_ATTRIB = 1800U;
static const uint16_t  DCI_MAX_FAILED_LOGIN_ATTEMPTS_MIN_ATTRIB = 3U;
static const uint16_t  DCI_MAX_FAILED_LOGIN_ATTEMPTS_MAX_ATTRIB = 10U;
static const int16_t  DCI_BLE_TEST_SETTING_5_MIN_ATTRIB = -500;
static const int16_t  DCI_BLE_TEST_SETTING_5_MAX_ATTRIB = 22000;
static const uint8_t  DCI_FW_UPGRADE_MODE_MIN_ATTRIB = 0U;
static const uint8_t  DCI_FW_UPGRADE_MODE_MAX_ATTRIB = 2U;
static const uint8_t  DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_MIN_ATTRIB = 0U;
static const uint8_t  DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_MAX_ATTRIB = 3U;
static const uint8_t  DCI_EXAMPLE_PWM_DUTYCYCLE_MIN_ATTRIB = 0U;
static const uint8_t  DCI_EXAMPLE_PWM_DUTYCYCLE_MAX_ATTRIB = 100U;
static const uint8_t  DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_MIN_ATTRIB = 1U;
static const uint8_t  DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_MAX_ATTRIB = 247U;
static const uint8_t  DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_MIN_ATTRIB = 0U;
static const uint8_t  DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_MAX_ATTRIB = 252U;
static const uint8_t  DCI_SAMPLE_APP_PARAMETER_MIN_ATTRIB = 0U;
static const uint8_t  DCI_SAMPLE_APP_PARAMETER_MAX_ATTRIB = 240U;
static const uint8_t  DCI_BACNET_DEVICE_ADDR_MIN_ATTRIB = 0U;
static const uint8_t  DCI_BACNET_DEVICE_ADDR_MAX_ATTRIB = 127U;
static const uint8_t  DCI_SNTP_SERVICE_ENABLE_MIN_ATTRIB = 0U;
static const uint8_t  DCI_SNTP_SERVICE_ENABLE_MAX_ATTRIB = 1U;
static const uint8_t  DCI_SNTP_ACTIVE_SERVER_MIN_ATTRIB = 0U;
static const uint8_t  DCI_SNTP_ACTIVE_SERVER_MAX_ATTRIB = 3U;
static const uint8_t  DCI_MAX_CONCURRENT_SESSION_MIN_ATTRIB = 1U;
static const uint8_t  DCI_MAX_CONCURRENT_SESSION_MAX_ATTRIB = 3U;
static const uint8_t  DCI_DEVICE_PWD_SETTING_MIN_ATTRIB = 0U;
static const uint8_t  DCI_DEVICE_PWD_SETTING_MAX_ATTRIB = 2U;
static const int8_t  DCI_BLE_TEST_SETTING_2_MIN_ATTRIB = -128;
static const int8_t  DCI_BLE_TEST_SETTING_2_MAX_ATTRIB = 127;
static const dci_bool_t  DCI_PHY1_FULL_DUPLEX_ENABLE_MIN_ATTRIB = False;
static const dci_bool_t  DCI_PHY1_FULL_DUPLEX_ENABLE_MAX_ATTRIB = True;
static const dci_bool_t  DCI_PHY1_FULL_DUPLEX_ACTUAL_MIN_ATTRIB = False;
static const dci_bool_t  DCI_PHY1_FULL_DUPLEX_ACTUAL_MAX_ATTRIB = True;
static const dci_bool_t  DCI_PHY1_AUTONEG_ENABLE_MIN_ATTRIB = False;
static const dci_bool_t  DCI_PHY1_AUTONEG_ENABLE_MAX_ATTRIB = True;
static const dci_bool_t  DCI_PHY1_PORT_ENABLE_MIN_ATTRIB = False;
static const dci_bool_t  DCI_PHY1_PORT_ENABLE_MAX_ATTRIB = True;
static const dci_bool_t  DCI_PHY2_FULL_DUPLEX_ENABLE_MIN_ATTRIB = False;
static const dci_bool_t  DCI_PHY2_FULL_DUPLEX_ENABLE_MAX_ATTRIB = True;
static const dci_bool_t  DCI_PHY2_FULL_DUPLEX_ACTUAL_MIN_ATTRIB = False;
static const dci_bool_t  DCI_PHY2_FULL_DUPLEX_ACTUAL_MAX_ATTRIB = True;
static const dci_bool_t  DCI_PHY2_AUTONEG_ENABLE_MIN_ATTRIB = False;
static const dci_bool_t  DCI_PHY2_AUTONEG_ENABLE_MAX_ATTRIB = True;
static const dci_bool_t  DCI_PHY2_PORT_ENABLE_MIN_ATTRIB = False;
static const dci_bool_t  DCI_PHY2_PORT_ENABLE_MAX_ATTRIB = True;
static const dci_bool_t  DCI_ETH_ACD_ENABLE_MIN_ATTRIB = False;
static const dci_bool_t  DCI_ETH_ACD_ENABLE_MAX_ATTRIB = True;
static const dci_bool_t  DCI_IP_BIT_NOT_CONNECTED_MIN_ATTRIB = False;
static const dci_bool_t  DCI_IP_BIT_NOT_CONNECTED_MAX_ATTRIB = True;
static const dci_bool_t  DCI_OP_BIT_NOT_CONNECTED_MIN_ATTRIB = False;
static const dci_bool_t  DCI_OP_BIT_NOT_CONNECTED_MAX_ATTRIB = True;
static const dci_bool_t  DCI_DEVICE_PARAMETERIZATION_ENABLE_MIN_ATTRIB = False;
static const dci_bool_t  DCI_DEVICE_PARAMETERIZATION_ENABLE_MAX_ATTRIB = True;
static const dci_bool_t  DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_MIN_ATTRIB = False;
static const dci_bool_t  DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_MAX_ATTRIB = True;


//*******************************************************
//******     The Range/Enum Block Array
//*******************************************************
const DCI_RANGE_BLOCK DCI_RANGE_BLOCK_ARRAY[TOTAL_RANGE_ARRAY_SIZE]= {
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_FW_UPGRADE_MODE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_FW_UPGRADE_MODE_MAX_ATTRIB ),
    DCI_FW_UPGRADE_MODE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_PHY1_LINK_SPEED_SELECT_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY1_FULL_DUPLEX_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY1_FULL_DUPLEX_ENABLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY1_FULL_DUPLEX_ACTUAL_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY1_FULL_DUPLEX_ACTUAL_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY1_AUTONEG_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY1_AUTONEG_ENABLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY1_PORT_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY1_PORT_ENABLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_PHY2_LINK_SPEED_SELECT_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY2_FULL_DUPLEX_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY2_FULL_DUPLEX_ENABLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY2_FULL_DUPLEX_ACTUAL_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY2_FULL_DUPLEX_ACTUAL_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY2_AUTONEG_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY2_AUTONEG_ENABLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY2_PORT_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_PHY2_PORT_ENABLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_ETH_ACD_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_ETH_ACD_ENABLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_EXAMPLE_PWM_DUTYCYCLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_EXAMPLE_PWM_DUTYCYCLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_EXAMPLE_MODBUS_SERIAL_PARITY_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_HTTP_ENABLE_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_IP_BIT_NOT_CONNECTED_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_IP_BIT_NOT_CONNECTED_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_OP_BIT_NOT_CONNECTED_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_OP_BIT_NOT_CONNECTED_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DEVICE_PARAMETERIZATION_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DEVICE_PARAMETERIZATION_ENABLE_MAX_ATTRIB ),
    DCI_DEVICE_PARAMETERIZATION_ENABLE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_PRODUCT_ACTIVE_FAULT_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SAMPLE_APP_OUTPUT_WORD_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SAMPLE_APP_OUTPUT_WORD_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SAMPLE_APP_OUTPUT_DWORD_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SAMPLE_APP_OUTPUT_DWORD_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SAMPLE_APP_PARAMETER_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SAMPLE_APP_PARAMETER_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_1_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_1_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_2_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_2_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_3_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_3_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_4_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_4_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_5_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_5_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_6_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_6_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_7_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_7_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_8_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_8_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_1_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_1_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_2_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_2_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_3_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_3_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_4_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_4_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_BACNET_IP_UPD_PORT_NUM_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BACNET_IP_FOREIGN_DEVICE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BACNET_IP_BBMD_PORT_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_IP_COMM_TIMEOUT_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_IP_COMM_TIMEOUT_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_BACNET_IP_PROTOCOL_STATUS_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BACNET_IP_FAULT_BEHAVIOR_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_ENABLE_BACNET_IP_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_DEVICE_ADDR_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_DEVICE_ADDR_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_IP_INSTANCE_NUM_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_IP_INSTANCE_NUM_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_MSTP_INSTANCE_NUM_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_MSTP_INSTANCE_NUM_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_MSTP_COMM_TIMEOUT_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BACNET_MSTP_COMM_TIMEOUT_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_BACNET_MSTP_BAUD_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BACNET_MSTP_PARITY_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BACNET_MSTP_STOPBIT_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BACNET_MSTP_PROTOCOL_STATUS_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BACNET_FAULT_CODE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_TIME_OFFSET_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_DATE_FORMAT_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_TIME_FORMAT_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_REST_RESET_COMMAND_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_DEVICE_CERT_VALID_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_DEVICE_CERT_CONTROL_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SNTP_SERVICE_ENABLE_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SNTP_SERVICE_ENABLE_MAX_ATTRIB ),
    DCI_SNTP_SERVICE_ENABLE_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SNTP_UPDATE_TIME_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SNTP_UPDATE_TIME_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SNTP_RETRY_TIME_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SNTP_RETRY_TIME_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SNTP_ACTIVE_SERVER_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_SNTP_ACTIVE_SERVER_MAX_ATTRIB ),
    DCI_SNTP_ACTIVE_SERVER_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_IOT_UPDATE_RATE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_IOT_CADENCE_UPDATE_RATE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_GROUP0_CADENCE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_GROUP1_CADENCE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_GROUP2_CADENCE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_GROUP3_CADENCE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_IOT_CONN_STAT_REASON_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_CHANGE_BASED_LOGGING_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_MIN_VAL_CONN_INTERVAL_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_MIN_VAL_CONN_INTERVAL_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_MAX_VAL_CONN_INTERVAL_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_MAX_VAL_CONN_INTERVAL_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_SLAVE_LATENCY_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_SLAVE_LATENCY_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_SUPERVISION_TIMEOUT_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_SUPERVISION_TIMEOUT_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_2_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_2_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_C_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_C_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_E_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_E_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_BLE_TEST_SETTING_F_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_READING_2_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_READING_2_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_READING_3_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_READING_3_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_5_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_5_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_6_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_6_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_7_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_SETTING_7_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_PRESENT_STATUS_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_PRESENT_STATUS_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_EVENT_LIST_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_BLE_TEST_EVENT_LIST_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_LOGGING_EVENT_CODE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_DEVICE_STATE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_NV_MEM_ID_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_NV_FAIL_OPERATION_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_LOG_PROC_IMAGE_ID_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_MODBUS_TCP_ENABLE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BACNET_IP_ENABLE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_LOG_PORT_NUMBER_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BLE_USER1_ROLE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BLE_USER2_ROLE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BLE_USER3_ROLE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_BLE_USER4_ROLE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_RESET_SOURCE_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_MAX_CONCURRENT_SESSION_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_MAX_CONCURRENT_SESSION_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_USER_INACTIVITY_TIMEOUT_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_USER_INACTIVITY_TIMEOUT_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_ABSOLUTE_TIMEOUT_SEC_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_ABSOLUTE_TIMEOUT_SEC_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_USER_LOCK_TIME_SEC_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_USER_LOCK_TIME_SEC_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_MAX_FAILED_LOGIN_ATTEMPTS_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_MAX_FAILED_LOGIN_ATTEMPTS_MAX_ATTRIB ),
    0xFFFF // no index allocated
},
{
    nullptr,
    nullptr,
    DCI_CORS_TYPE_ENUM_INDEX
},
{
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DEVICE_PWD_SETTING_MIN_ATTRIB ),
    reinterpret_cast<DCI_RANGE_DATA_TD const*>( &DCI_DEVICE_PWD_SETTING_MAX_ATTRIB ),
    DCI_DEVICE_PWD_SETTING_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_LANG_PREF_SETTING_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_PTP_ENABLE_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_CLIENT_AP_SECURITY_ENUM_INDEX
},
{
    nullptr,
    nullptr,
    DCI_CLIENT_AP_BAND_ENUM_INDEX
}
};


//*******************************************************
//******     The Default Block
//*******************************************************
static const float64_t DCI_BLE_TEST_READING_3_DEFAULT_ATTRIB = 558;
static const float64_t DCI_IOT_ActiveEnergyL1_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ActiveEnergyL2_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ReactiveEnergyL1_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ReactiveEnergyL2_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ApparentEnergyL1_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ApparentEnergyL2_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ReverseActiveEnergyL1_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ReverseReactiveEnergyL1_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ReverseApparentEnergyL1_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ReverseActiveEnergyL2_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ReverseReactiveEnergyL2_DEFAULT_ATTRIB = 0;
static const float64_t DCI_IOT_ReverseApparentEnergyL2_DEFAULT_ATTRIB = 0;
static const uint64_t DCI_BLE_TEST_SETTING_C_DEFAULT_ATTRIB = 54321456UL;
static const int64_t DCI_BLE_TEST_EVENT_LIST_DEFAULT_ATTRIB = -123;
static const int64_t DCI_SNTP_TIME_OFFSET_DEBUG_DEFAULT_ATTRIB = 0;
static const float32_t DCI_BLE_TEST_READING_2_DEFAULT_ATTRIB = 500.0F;
static const float32_t DCI_IOT_VoltageL1_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_VoltageL2_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_CurrentL1_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_CurrentL2_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_ActivePowerL1_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_ActivePowerL2_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_ReactivePowerL1_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_ReactivePowerL2_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_ApparentPowerL1_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_ApparentPowerL2_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_PowerFactorL1_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_PowerFactorL2_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_FrequencyL1_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_FrequencyL2_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_Load_Percentage_DEFAULT_ATTRIB = 0.0F;
static const float32_t DCI_IOT_TEMPERATURE_DEFAULT_ATTRIB = 0.0F;
static const uint32_t DCI_APP_FIRM_VER_NUM_DEFAULT_ATTRIB = CONST_FIRMWARE_VER_NUM;
static const uint32_t DCI_APP_FIRM_UPGRADE_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_MULTICAST_ENABLE_DEFAULT_ATTRIB = 0x00000010U;
static const uint32_t DCI_BROADCAST_ENABLE_DEFAULT_ATTRIB = 0x00000000U;
static const uint32_t DCI_EXAMPLE_MODBUS_SERIAL_BAUD_DEFAULT_ATTRIB = 19200U;
static const uint32_t DCI_RTC_TIME_DEFAULT_ATTRIB = 0x0034220AU;
static const uint32_t DCI_RTC_DATE_DEFAULT_ATTRIB = 0x07E00319U;
static const uint32_t DCI_USER_1_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_1_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_2_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_2_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_3_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_3_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_4_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_4_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_5_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_5_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_6_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_USER_6_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_SAMPLE_APP_OUTPUT_DWORD_DEFAULT_ATTRIB = 1U;
static const uint32_t DCI_SAMPLE_ACYCLIC_PARAMETER_1_DEFAULT_ATTRIB = 0x11223344U;
static const uint32_t DCI_SAMPLE_ACYCLIC_PARAMETER_5_DEFAULT_ATTRIB = 0x05500550U;
static const uint32_t DCI_ETH_NAME_SERVER_1_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_ETH_NAME_SERVER_2_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_DYN_IN_ASM_TERMINATOR_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_DYN_IN_ASM_MEMBER_1_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_IN_ASM_MEMBER_2_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_IN_ASM_MEMBER_3_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_IN_ASM_MEMBER_4_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_IN_ASM_MEMBER_5_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_IN_ASM_MEMBER_6_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_IN_ASM_MEMBER_7_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_IN_ASM_MEMBER_8_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_OUT_ASM_TERMINATOR_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_DYN_OUT_ASM_MEMBER_1_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_OUT_ASM_MEMBER_2_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_OUT_ASM_MEMBER_3_DEFAULT_ATTRIB = 0x00000001U;
static const uint32_t DCI_DYN_OUT_ASM_MEMBER_4_DEFAULT_ATTRIB = 0x00000001U;
static uint32_t DCI_BACNET_IP_INSTANCE_NUM_DEFAULT_ATTRIB = 0U;
static uint32_t DCI_BACNET_MSTP_INSTANCE_NUM_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_BACNET_MSTP_BAUD_DEFAULT_ATTRIB = 38400U;
static const uint32_t DCI_BACNET_DATABASE_REVISION_DEFAULT_ATTRIB = 1U;
static const uint32_t DCI_AV_ANY_PARAM_VAL_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_UNIX_EPOCH_TIME_DEFAULT_ATTRIB = 946684800U;
static const uint32_t DCI_UNIX_EPOCH_TIME_64_BIT_DEFAULT_ATTRIB[2] = 
        { 946684800U, 0U };
static const uint32_t DCI_SNTP_UPDATE_TIME_DEFAULT_ATTRIB = 3600000U;
static const uint32_t DCI_LOG_INTERVAL_TIME_MS_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_BLE_TEST_SETTING_B_DEFAULT_ATTRIB = 0x34566787U;
static const uint32_t DCI_BLE_TEST_SETTING_7_DEFAULT_ATTRIB = 2000U;
static const uint32_t DCI_BLE_TEST_DEVICE_TIME_DEFAULT_ATTRIB = 1514955660U;
static const uint32_t DCI_CR_PRIORITY_TIMING_DEFAULT_ATTRIB[6] = 
        { 0U, 0U, 0U, 0U, 0U, 0U };
static const uint32_t DCI_OS_PRIORITY_TIMING_DEFAULT_ATTRIB[8] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint32_t DCI_RESET_LINE_NUMBER_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_RESET_OS_TASK_PRIORITY_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_RESET_OS_STACK_SIZE_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_RESET_CR_TASK_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_RESET_CR_TASK_PARAM_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_RESET_CSTACK_SIZE_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_RESET_HIGHEST_OS_STACK_SIZE_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_RESET_HEAP_USAGE_PERCENT_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_RESET_SOURCE_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_ABSOLUTE_TIMEOUT_SEC_DEFAULT_ATTRIB = 21600U;
static const uint32_t DCI_J1939_MSG_EX9_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_J1939_MSG_EX10_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_BLE_LOG_INDEX_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_DEVICE_UP_TIME_SEC_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_PTP_SECONDS_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_PTP_NANOSECONDS_TIME_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_IEC61850_CONFIG_REV1_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_IEC61850_CONFIG_REV2_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_IEC61850_CONFIG_REV4_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_IEC61850_CONFIG_REV5_DEFAULT_ATTRIB = 0U;
static const uint32_t DCI_IEC61850_SAV_CONFIG_REV1_DEFAULT_ATTRIB = 1U;
static const uint32_t DCI_IOT_POWER_UP_STATE_DEFAULT_ATTRIB = 1U;
static const int32_t DCI_GROUP0_CADENCE_DEFAULT_ATTRIB = 10000;
static const int32_t DCI_GROUP1_CADENCE_DEFAULT_ATTRIB = 60000;
static const int32_t DCI_GROUP2_CADENCE_DEFAULT_ATTRIB = 1000;
static const int32_t DCI_GROUP3_CADENCE_DEFAULT_ATTRIB = 1000;
static const int32_t DCI_BLE_DEVICE_PASSKEY_DEFAULT_ATTRIB = 123456;
static const int32_t DCI_BLE_TEST_SETTING_6_DEFAULT_ATTRIB = -600;
static const int32_t DCI_CLIENT_AP_SECURITY_DEFAULT_ATTRIB = 6;
static const uint16_t DCI_PRODUCT_CODE_DEFAULT_ATTRIB = CONST_PROD_CODE;
static const uint16_t DCI_RESET_COUNTER_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_PWR_BRD_DIP_SWITCHES_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_PHY1_LINK_SPEED_SELECT_DEFAULT_ATTRIB = 100U;
static const uint16_t DCI_PHY2_LINK_SPEED_SELECT_DEFAULT_ATTRIB = 100U;
static const uint16_t DCI_MODBUS_TCP_COMM_TIMEOUT_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_MODBUS_SERIAL_COMM_TIMEOUT_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_SIMPLE_APP_CONTROL_WORD_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_SIMPLE_APP_STATUS_WORD_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_MEMBERS_ASSY_0_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_1_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_2_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_3_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_4_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_5_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_6_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_7_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_8_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_MEMBERS_ASSY_9_DEFAULT_ATTRIB[10] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint16_t DCI_EXAMPLE_PWM_FREQUENCY_DEFAULT_ATTRIB = 1000U;
static const uint16_t DCI_EXAMPLE_MODBUS_SERIAL_PARITY_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_VALID_BITS_USERS_DEFAULT_ATTRIB = 1U;
static const uint16_t DCI_USER_1_PWD_AGING_DEFAULT_ATTRIB = 1U;
static const uint16_t DCI_USER_2_PWD_AGING_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_USER_3_PWD_AGING_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_USER_4_PWD_AGING_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_USER_5_PWD_AGING_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_USER_6_PWD_AGING_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_PRODUCT_ACTIVE_FAULT_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_SAMPLE_APP_OUTPUT_WORD_DEFAULT_ATTRIB = 1U;
static const uint16_t DCI_SAMPLE_ACYCLIC_PARAMETER_2_DEFAULT_ATTRIB = 0x55AAU;
static const uint16_t DCI_SAMPLE_ACYCLIC_PARAMETER_6_DEFAULT_ATTRIB = 0xAA55U;
static const uint16_t DCI_CIP_VENDOR_ID_DEFAULT_ATTRIB = 68U;
static const uint16_t DCI_CIP_DEVICE_TYPE_DEFAULT_ATTRIB = 0x2BU;
static const uint16_t DCI_CIP_DEVICE_IDENT_DEV_STATUS_DEFAULT_ATTRIB = 0x74U;
static const uint16_t DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_DEFAULT_ATTRIB = 120U;
static const uint16_t DCI_DYN_IN_ASM_SELECT_1_DEFAULT_ATTRIB = 1U;
static const uint16_t DCI_DYN_IN_ASM_SELECT_2_DEFAULT_ATTRIB = 12U;
static const uint16_t DCI_DYN_IN_ASM_SELECT_3_DEFAULT_ATTRIB = 2U;
static const uint16_t DCI_DYN_IN_ASM_SELECT_4_DEFAULT_ATTRIB = 3U;
static const uint16_t DCI_DYN_IN_ASM_SELECT_5_DEFAULT_ATTRIB = 4U;
static const uint16_t DCI_DYN_IN_ASM_SELECT_6_DEFAULT_ATTRIB = 6U;
static const uint16_t DCI_DYN_IN_ASM_SELECT_7_DEFAULT_ATTRIB = 7U;
static const uint16_t DCI_DYN_IN_ASM_SELECT_8_DEFAULT_ATTRIB = 8U;
static const uint16_t DCI_DYN_OUT_ASM_SELECT_1_DEFAULT_ATTRIB = 1U;
static const uint16_t DCI_DYN_OUT_ASM_SELECT_2_DEFAULT_ATTRIB = 2U;
static const uint16_t DCI_DYN_OUT_ASM_SELECT_3_DEFAULT_ATTRIB = 3U;
static const uint16_t DCI_DYN_OUT_ASM_SELECT_4_DEFAULT_ATTRIB = 4U;
static const uint16_t DCI_BACNET_IP_UPD_PORT_NUM_DEFAULT_ATTRIB = 47808U;
static const uint16_t DCI_BACNET_IP_BBMD_PORT_DEFAULT_ATTRIB = 47808U;
static const uint16_t DCI_BACNET_IP_REGISTRATION_INTERVAL_DEFAULT_ATTRIB = 10U;
static const uint16_t DCI_BACNET_IP_COMM_TIMEOUT_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_BACNET_MSTP_COMM_TIMEOUT_DEFAULT_ATTRIB = 10000U;
static const uint16_t DCI_BACNET_MSTP_PARITY_DEFAULT_ATTRIB = 2U;
static const uint16_t DCI_BACNET_MSTP_STOPBIT_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_ANALOG_VALUE_0_DEFAULT_ATTRIB = 11U;
static const uint16_t DCI_ANALOG_VALUE_1_DEFAULT_ATTRIB = 22U;
static const uint16_t DCI_AV_ANY_PARAM_NUM_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_ANALOG_INPUT_0_DEFAULT_ATTRIB = 33U;
static const uint16_t DCI_ANALOG_INPUT_1_DEFAULT_ATTRIB = 44U;
static const uint16_t DCI_DIGITAL_INPUT_WORD_DEFAULT_ATTRIB = 55U;
static const uint16_t DCI_DIGITAL_OUTPUT_WORD_DEFAULT_ATTRIB = 66U;
static const uint16_t DCI_SNTP_RETRY_TIME_DEFAULT_ATTRIB = 15000U;
static const uint16_t DCI_IOT_PROXY_PORT_DEFAULT_ATTRIB = 8080U;
static const uint16_t DCI_BLE_DEVICE_BONDING_INFO_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_BLE_MIN_VAL_CONN_INTERVAL_DEFAULT_ATTRIB = 16U;
static const uint16_t DCI_BLE_MAX_VAL_CONN_INTERVAL_DEFAULT_ATTRIB = 32U;
static const uint16_t DCI_BLE_SLAVE_LATENCY_DEFAULT_ATTRIB = 4U;
static const uint16_t DCI_BLE_SUPERVISION_TIMEOUT_DEFAULT_ATTRIB = 200U;
static const uint16_t DCI_BLE_TEST_SETTING_A_DEFAULT_ATTRIB = 0x5634U;
static const uint16_t DCI_BLE_TEST_SETTING_E_DEFAULT_ATTRIB = 5U;
static const uint16_t DCI_BLE_TEST_PRESENT_STATUS_DEFAULT_ATTRIB = 500U;
static const uint16_t DCI_USER_INACTIVITY_TIMEOUT_DEFAULT_ATTRIB = 900U;
static const uint16_t DCI_USER_LOCK_TIME_SEC_DEFAULT_ATTRIB = 300U;
static const uint16_t DCI_MAX_FAILED_LOGIN_ATTEMPTS_DEFAULT_ATTRIB = 10U;
static const uint16_t DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_MDNS_SERVER1_PORT_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_J1939_MSG_EX4_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_J1939_MSG_EX5_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_J1939_MSG_EX6_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_J1939_MSG_EX8_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_BLE_TEST_COUNTER_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_DATA_LOG_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_EVENT_LOG_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_AUDIT_POWER_LOG_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_AUDIT_FW_UPGRADE_LOG_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_AUDIT_USER_LOG_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_AUDIT_CONFIG_LOG_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_IEC61850_VLAN_VID1_DEFAULT_ATTRIB = 2U;
static const uint16_t DCI_IEC61850_VLAN_VID2_DEFAULT_ATTRIB = 2U;
static const uint16_t DCI_IEC61850_VLAN_VID4_DEFAULT_ATTRIB = 2U;
static const uint16_t DCI_IEC61850_VLAN_VID5_DEFAULT_ATTRIB = 2U;
static const uint16_t DCI_IEC61850_SAV_VLAN_VID1_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_IEC61850_SAV_VLAN_APP_ID1_DEFAULT_ATTRIB = 0x4001U;
static const uint16_t DCI_IEC61850_SAV_SMP_RATE1_DEFAULT_ATTRIB = 256U;
static const uint16_t DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC1_DEFAULT_ATTRIB = 60U;
static const uint16_t DCI_IEC61850_SAV_NUM_ASDU1_DEFAULT_ATTRIB = 8U;
static const uint16_t DCI_IEC61850_SAV_VLAN_VID2_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_IEC61850_SAV_VLAN_APP_ID2_DEFAULT_ATTRIB = 0x5001U;
static const uint16_t DCI_IEC61850_SAV_SMP_SYNC2_DEFAULT_ATTRIB = 0U;
static const uint16_t DCI_IEC61850_SAV_SMP_MODE2_DEFAULT_ATTRIB = 1U;
static const uint16_t DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC2_DEFAULT_ATTRIB = 60U;
static const uint16_t DCI_NV_MAP_VER_NUM_DEFAULT_ATTRIB = CONST_DCI_NV_MAP_VERSION_NUM;
static const int16_t DCI_TIME_OFFSET_DEFAULT_ATTRIB = 0;
static const int16_t DCI_BLE_TEST_SETTING_5_DEFAULT_ATTRIB = -500;
static const int16_t DCI_IEC61850_VLAN_APP_ID1_DEFAULT_ATTRIB = 1001;
static const int16_t DCI_IEC61850_VLAN_APP_ID2_DEFAULT_ATTRIB = 1002;
static const int16_t DCI_IEC61850_VLAN_APP_ID4_DEFAULT_ATTRIB = 2001;
static const int16_t DCI_IEC61850_VLAN_APP_ID5_DEFAULT_ATTRIB = 2002;
static const int16_t DCI_IEC61850_SAV_SMP_MODE1_DEFAULT_ATTRIB = 1;
static const int16_t DCI_IEC61850_SAV_SMP_RATE2_DEFAULT_ATTRIB = 256;
static const uint8_t DCI_VENDOR_NAME_DEFAULT_ATTRIB[12] = 
        { "Eaton Corp." };
static const uint8_t DCI_PRODUCT_CODE_ASCII_DEFAULT_ATTRIB[7] = 
        CONST_PROD_CODE_ASCII;
static const uint8_t DCI_VENDOR_URL_DEFAULT_ATTRIB[14] = 
        { "www.eaton.com" };
static const uint8_t DCI_PRODUCT_NAME_DEFAULT_ATTRIB[32] = 
        { "SmartBreaker-2.0" };
static const uint8_t DCI_MODEL_NAME_DEFAULT_ATTRIB[10] = 
        { "ESB2" };
static const uint8_t DCI_USER_APP_NAME_DEFAULT_ATTRIB[32] = 
        { "SmartBreaker-2.0" };
static const uint8_t DCI_FW_UPGRADE_MODE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_LAST_APP_FIRM_UPGRADE_USER_DEFAULT_ATTRIB[21] = 
        { "admin" };
static const uint8_t DCI_LAST_WEB_FIRM_UPGRADE_USER_DEFAULT_ATTRIB[21] = 
        { "admin" };
static const uint8_t DCI_WEB_FIRM_UPGRADE_STATUS_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_HEAP_USED_MAX_PERC_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_ETHERNET_MAC_ADDRESS_DEFAULT_ATTRIB[6] = 
        { "" };
static const uint8_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_IP_ADDRESS_ALLOCATION_METHOD_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_STATIC_IP_ADDRESS_DEFAULT_ATTRIB[4] = 
        { 0xFEU, 0x01U, 0xA8U, 0xC0U };
static const uint8_t DCI_STATIC_SUBNET_MASK_DEFAULT_ATTRIB[4] = 
        { 0x00U, 0xFFU, 0xFFU, 0xFFU };
static const uint8_t DCI_STATIC_DEFAULT_GATEWAY_DEFAULT_ATTRIB[4] = 
        { 0x01U, 0x01U, 0xA8U, 0xC0U };
static const uint8_t DCI_ETH_ACD_CONFLICTED_STATE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_ETH_ACD_CONFLICTED_MAC_DEFAULT_ATTRIB[6] = 
        { 0U, 0U, 0U, 0U, 0U, 0U };
static const uint8_t DCI_ETH_ACD_CONFLICTED_ARP_PDU_DEFAULT_ATTRIB[28] = 
        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint8_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_VALID_BITS_ASSY_0_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_1_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_2_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_3_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_4_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_5_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_6_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_7_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_8_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_VALID_BITS_ASSY_9_DEFAULT_ATTRIB[2] = 
        { 0U, 0U };
static const uint8_t DCI_LTK_VER_ASCII_DEFAULT_ATTRIB[19] = 
        CONST_LTK_VER_ASCII;
static const uint8_t DCI_EXAMPLE_PWM_DUTYCYCLE_DEFAULT_ATTRIB = 50U;
static const uint8_t DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_USER_WEB_FW_NAME_DEFAULT_ATTRIB[32] = 
        { "RTK_WEB_IMAGE" };
static const uint8_t DCI_PRODUCT_PROC_NAME_DEFAULT_ATTRIB[32] = 
        { "RTK_MAIN_PROCESSOR" };
static const uint8_t DCI_USER_1_FULLNAME_DEFAULT_ATTRIB[33] = 
        { "Administrator" };
static const uint8_t DCI_USER_1_USERNAME_DEFAULT_ATTRIB[21] = 
        { "admin" };
static const uint8_t DCI_USER_1_PWD_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_1_ROLE_DEFAULT_ATTRIB = 99U;
static const uint8_t DCI_USER_1_PWD_COMPLEXITY_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_2_FULLNAME_DEFAULT_ATTRIB[33] = 
        { "" };
static const uint8_t DCI_USER_2_USERNAME_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_2_PWD_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_2_ROLE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_2_PWD_COMPLEXITY_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_3_FULLNAME_DEFAULT_ATTRIB[33] = 
        { "" };
static const uint8_t DCI_USER_3_USERNAME_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_3_PWD_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_3_ROLE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_3_PWD_COMPLEXITY_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_4_FULLNAME_DEFAULT_ATTRIB[33] = 
        { "" };
static const uint8_t DCI_USER_4_USERNAME_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_4_PWD_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_4_ROLE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_4_PWD_COMPLEXITY_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_5_FULLNAME_DEFAULT_ATTRIB[33] = 
        { "" };
static const uint8_t DCI_USER_5_USERNAME_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_5_PWD_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_5_ROLE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_5_PWD_COMPLEXITY_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_6_FULLNAME_DEFAULT_ATTRIB[33] = 
        { "" };
static const uint8_t DCI_USER_6_USERNAME_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_6_PWD_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_USER_6_ROLE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_USER_6_PWD_COMPLEXITY_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_SAMPLE_APP_OUTPUT_BIT_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_SAMPLE_APP_PARAMETER_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_SAMPLE_ACYCLIC_PARAMETER_3_DEFAULT_ATTRIB = 0x11U;
static const uint8_t DCI_SAMPLE_ACYCLIC_PARAMETER_4_DEFAULT_ATTRIB = 0x22U;
static const uint8_t DCI_ETH_DOMAIN_NAME_DEFAULT_ATTRIB[48] = 
        { "" };
static const uint8_t DCI_ETH_HOST_NAME_DEFAULT_ATTRIB[64] = 
        { "" };
static const uint8_t DCI_CIP_DEVICE_IDENT_STATE_DEFAULT_ATTRIB = 3U;
static const uint8_t DCI_BACNET_VENDOR_IDENTIFIER_DEFAULT_ATTRIB = 191U;
static const uint8_t DCI_MAX_MASTER_DEFAULT_ATTRIB = 127U;
static const uint8_t DCI_MAX_INFO_FRAMES_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_PROFILE_NAME_DEFAULT_ATTRIB[32] = 
        { "B-ASC" };
static const uint8_t DCI_BACNET_IP_FOREIGN_DEVICE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_BACNET_IP_BBMD_IP_DEFAULT_ATTRIB[4] = 
        { 0U, 0U, 0U, 0U };
static const uint8_t DCI_BACNET_IP_FAULT_BEHAVIOR_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_ENABLE_BACNET_IP_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_BACNET_DEVICE_ADDR_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_MULTI_STATE_0_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_MULTI_STATE_1_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_TRUSTED_IP_WHITELIST_DEFAULT_ATTRIB[12] = 
        { 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0x01U, 0xA8U, 0xC0U, 0xFFU, 0xFFU, 0xFFU, 0xFFU };
static const uint8_t DCI_DATE_FORMAT_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_TIME_FORMAT_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_DEVICE_CERT_VALID_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_DEVICE_CERT_CONTROL_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_SNTP_SERVICE_ENABLE_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_SNTP_SERVER_1_DEFAULT_ATTRIB[40] = 
        { "pool.ntp.org" };
static const uint8_t DCI_SNTP_SERVER_2_DEFAULT_ATTRIB[40] = 
        { "151.110.232.100" };
static const uint8_t DCI_SNTP_SERVER_3_DEFAULT_ATTRIB[40] = 
        { "151.110.232.100" };
static const uint8_t DCI_IOT_PROXY_SERVER_DEFAULT_ATTRIB[20] = 
        { "proxy.apac.etn.com" };
static const uint8_t DCI_IOT_PROXY_USERNAME_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_IOT_PROXY_PASSWORD_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_IOT_DEVICE_GUID_DEFAULT_ATTRIB[37] = 
        { "" };
static const uint8_t DCI_IOT_DEVICE_PROFILE_DEFAULT_ATTRIB[37] = 
        { "58af4fd1-b1f8-ec11-b47a-0050f2f4e861" };
static const uint8_t DCI_IOT_CONN_STRING_DEFAULT_ATTRIB[170] = 
        { "" };
static const uint8_t DCI_IOT_UPDATE_RATE_DEFAULT_ATTRIB = 5U;
static const uint8_t DCI_IOT_CADENCE_UPDATE_RATE_DEFAULT_ATTRIB = 5U;
static const uint8_t DCI_IOT_CONN_STAT_REASON_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_CHANGE_BASED_LOGGING_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_BLE_DEVICE_SNAME_DEFAULT_ATTRIB[13] = 
        { "EATON_DEV_1" };
static const uint8_t DCI_BLE_DEVICE_PROFILE_ID_DEFAULT_ATTRIB[17] = 
        { "BLE_DEMO_PROFILE" };
static const uint8_t DCI_BLE_DEVICE_VENDOR_DEFAULT_ATTRIB[13] = 
        { "SILICON_LABS" };
static const uint8_t DCI_BLE_DEVICE_FAMILY_DEFAULT_ATTRIB[7] = 
        { "BGM111" };
static const uint8_t DCI_BLE_DEVICE_ROLE_DEFAULT_ATTRIB[11] = 
        { "PERIPHERAL" };
static const uint8_t DCI_BLE_DEVICE_MODEL_DEFAULT_ATTRIB[8] = 
        { "BLE_4_2" };
static const uint8_t DCI_BLE_DEVICE_LNAME_DEFAULT_ATTRIB[18] = 
        { "EATON CORPORATION" };
static const uint8_t DCI_BLE_DEVICE_HW_VER_DEFAULT_ATTRIB[8] = 
        { "REV_A03" };
static const uint8_t DCI_BLE_DEVICE_SW_VER_DEFAULT_ATTRIB[10] = 
        { "SDK_2_4_2" };
static const uint8_t DCI_BLE_USERNAME_DEFAULT_ATTRIB[32] = 
        { "ADMINISTRATOR" };
static const uint8_t DCI_BLE_PASSWORD_DEFAULT_ATTRIB[32] = 
        { "ADMINISTRATOR" };
static const uint8_t DCI_BLE_DEVICE_MANUFACTURER_NAME_DEFAULT_ATTRIB[11] = 
        { "BLUE GECKO" };
static const uint8_t DCI_BLE_DEVICE_SERIAL_NO_DEFAULT_ATTRIB[10] = 
        { "172230625" };
static const uint8_t DCI_BLE_DEVICE_IMAGE_URL_DEFAULT_ATTRIB[8] = 
        { "Desktop" };
static const uint8_t DCI_BLE_ACTIVE_ACCESS_LEVEL_DEFAULT_ATTRIB = 7U;
static const uint8_t DCI_BLE_TEST_SETTING_1_DEFAULT_ATTRIB = 0x45U;
static const uint8_t DCI_BLE_TEST_SETTING_3_DEFAULT_ATTRIB[6] = 
        { 0xA8U, 0xC0U, 0xADU, 0xABU, 0xEDU, 0x3BU };
static const uint8_t DCI_BLE_TEST_SETTING_D_DEFAULT_ATTRIB[4] = 
        { 0xA8U, 0xC0U, 0xADU, 0xABU };
static const uint8_t DCI_BLE_TEST_SETTING_F_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_BLE_TEST_READING_1_DEFAULT_ATTRIB[32] = 
        { "TEST STRING" };
static const uint8_t DCI_CPU_UTILIZATION_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_LOGGING_EVENT_CODE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_DEVICE_STATE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_LOGGED_USERNAME_DEFAULT_ATTRIB[21] = 
        { "" };
static const uint8_t DCI_BLE_ADMIN_PASSWORD_DEFAULT_ATTRIB[32] = 
        { "ADMINISTRATOR" };
static const uint8_t DCI_BLE_OBSERVER_PASSWORD_DEFAULT_ATTRIB[32] = 
        { "OBSERVER" };
static const uint8_t DCI_BLE_OPERATOR_PASSWORD_DEFAULT_ATTRIB[32] = 
        { "OPERATOR" };
static const uint8_t DCI_BLE_ENGINEER_PASSWORD_DEFAULT_ATTRIB[32] = 
        { "ENGINEER" };
static const uint8_t DCI_BLE_USER1_ROLE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_BLE_USER1_NAME_DEFAULT_ATTRIB[32] = 
        { "Observer" };
static const uint8_t DCI_BLE_USER1_PASSWORD_DEFAULT_ATTRIB[28] = 
        { "observer" };
static const uint8_t DCI_BLE_USER2_ROLE_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_BLE_USER2_NAME_DEFAULT_ATTRIB[32] = 
        { "Operator" };
static const uint8_t DCI_BLE_USER2_PASSWORD_DEFAULT_ATTRIB[28] = 
        { "operator" };
static const uint8_t DCI_BLE_USER3_ROLE_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_BLE_USER3_NAME_DEFAULT_ATTRIB[32] = 
        { "Engineer" };
static const uint8_t DCI_BLE_USER4_ROLE_DEFAULT_ATTRIB = 3U;
static const uint8_t DCI_BLE_USER4_NAME_DEFAULT_ATTRIB[32] = 
        { "Administrator" };
static const uint8_t DCI_RESET_FUNCTION_NAME_DEFAULT_ATTRIB[20] = 
        { "" };
static const uint8_t DCI_RESET_OS_TASK_NAME_DEFAULT_ATTRIB[20] = 
        { "" };
static const uint8_t DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DEFAULT_ATTRIB[20] = 
        { "" };
static const uint8_t DCI_MAX_CONCURRENT_SESSION_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_CORS_TYPE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_BLE_EDDYSTONE_URL_DEFAULT_ATTRIB[18] = 
        { 0x03U, 0x45U, 0x61U, 0x74U, 0x6FU, 0x6EU, 0x07U, 0x21U };
static const uint8_t DCI_BLE_IBEACON_MAJOR_NUMBER_DEFAULT_ATTRIB[2] = 
        { 0x11U, 0x22U };
static const uint8_t DCI_BLE_IBEACON_MINOR_NUMBER_DEFAULT_ATTRIB[2] = 
        { 0x33U, 0x44U };
static const uint8_t DCI_BLE_IBEACON_128_BIT_UUID_DEFAULT_ATTRIB[16] = 
        { 0xF7U, 0x82U, 0x6DU, 0xA6U, 0x4FU, 0xA2U, 0x4EU, 0x98U, 
0x80, 0x24U, 0xBCU, 0x5BU, 0x71U, 0xE0U, 0x89U, 0x3EU };
static const uint8_t DCI_MDNS_SERVER1_NAME_DEFAULT_ATTRIB[64] = 
        { "PxGreenHost0" };
static const uint8_t DCI_MDNS_SERVER1_IP_ADDR_DEFAULT_ATTRIB[4] = 
        { 0x00U, 0x00U, 0x00U, 0x00U };
static const uint8_t DCI_J1939_MSG_EX1_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_J1939_MSG_EX2_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_J1939_MSG_EX3_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_J1939_MSG_EX7_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_DEVICE_PWD_SETTING_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_LICENSE_INFORMATION_DEFAULT_ATTRIB[84] = 
        { "https://www.freertos.org/a00114.html;https://lwip.fandom.com/wiki/License" };
static const uint8_t DCI_DEVICE_DEFAULT_PWD_DEFAULT_ATTRIB[21] = 
        { "Admin*1" };
static const uint8_t DCI_LANG_PREF_SETTING_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_COMMON_LANG_PREF_DEFAULT_ATTRIB[6] = 
        { "en" };
static const uint8_t DCI_LAST_LANG_FIRM_UPGRADE_USER_DEFAULT_ATTRIB[21] = 
        { "admin" };
static const uint8_t DCI_IEC61850_VLAN_PRI1_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_IEC61850_MAC_ADD_ID1_DEFAULT_ATTRIB[6] = 
        { 0x01U, 0x0CU, 0xCDU, 0xBEU, 0xEFU, 0x11U };
static const uint8_t DCI_IEC61850_GOOSE_ID1_DEFAULT_ATTRIB[32] = 
        { "goID_SubTest1" };
static const uint8_t DCI_IEC61850_CB_REF_NAME1_DEFAULT_ATTRIB[32] = 
        { "Device_1/LLN0.gocbRef_1" };
static const uint8_t DCI_IEC61850_DATA_SET_NAME1_DEFAULT_ATTRIB[32] = 
        { "Device_1/datset_name_1" };
static const uint8_t DCI_IEC61850_NDS_COM1_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_TEST1_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_NUM_DATA_SET_ENTRIES1_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_IEC61850_VLAN_PRI2_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_IEC61850_MAC_ADD_ID2_DEFAULT_ATTRIB[6] = 
        { 0x01U, 0x0CU, 0xCDU, 0xBEU, 0xEFU, 0x22U };
static const uint8_t DCI_IEC61850_GOOSE_ID2_DEFAULT_ATTRIB[32] = 
        { "goID_SubTest2" };
static const uint8_t DCI_IEC61850_CB_REF_NAME2_DEFAULT_ATTRIB[32] = 
        { "Device_1/LLN0.gocbRef_2" };
static const uint8_t DCI_IEC61850_DATA_SET_NAME2_DEFAULT_ATTRIB[32] = 
        { "Device_1/datset_name_2" };
static const uint8_t DCI_IEC61850_NDS_COM2_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_TEST2_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_NUM_DATA_SET_ENTRIES2_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_IEC61850_VLAN_PRI4_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_IEC61850_MAC_ADD_ID4_DEFAULT_ATTRIB[6] = 
        { 0x01U, 0x0CU, 0xCDU, 0xBEU, 0xEFU, 0x21U };
static const uint8_t DCI_IEC61850_GOOSE_ID4_DEFAULT_ATTRIB[32] = 
        { "goID_SubTest1" };
static const uint8_t DCI_IEC61850_CB_REF_NAME4_DEFAULT_ATTRIB[32] = 
        { "Device_2/LLN0.gocbRef_1" };
static const uint8_t DCI_IEC61850_DATA_SET_NAME4_DEFAULT_ATTRIB[32] = 
        { "Device_2/datset_name_1" };
static const uint8_t DCI_IEC61850_NDS_COM4_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_TEST4_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_NUM_DATA_SET_ENTRIES4_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_IEC61850_VLAN_PRI5_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_IEC61850_MAC_ADD_ID5_DEFAULT_ATTRIB[6] = 
        { 0x01U, 0x0CU, 0xCDU, 0xBEU, 0xEFU, 0x22U };
static const uint8_t DCI_IEC61850_GOOSE_ID5_DEFAULT_ATTRIB[32] = 
        { "goID_SubTest2" };
static const uint8_t DCI_IEC61850_CB_REF_NAME5_DEFAULT_ATTRIB[32] = 
        { "Device_2/LLN0.gocbRef_2" };
static const uint8_t DCI_IEC61850_DATA_SET_NAME5_DEFAULT_ATTRIB[32] = 
        { "Device_2/datset_name_2" };
static const uint8_t DCI_IEC61850_NDS_COM5_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_TEST5_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_NUM_DATA_SET_ENTRIES5_DEFAULT_ATTRIB = 2U;
static const uint8_t DCI_IEC61850_SAV_VLAN_PRI1_DEFAULT_ATTRIB = 4U;
static const uint8_t DCI_IEC61850_SAV_MAC_ADD_ID1_DEFAULT_ATTRIB[6] = 
        { 0x01U, 0xFEU, 0xEDU, 0xBEU, 0xEFU, 0x11U };
static const uint8_t DCI_IEC61850_SAV_ID1_DEFAULT_ATTRIB[16] = 
        { "MU01" };
static const uint8_t DCI_IEC61850_SAV_CB_REF_NAME1_DEFAULT_ATTRIB[32] = 
        { "Device_1/LLN0.svbRef_1" };
static const uint8_t DCI_IEC61850_SAV_DATA_SET_NAME1_DEFAULT_ATTRIB[16] = 
        { "PhsMeas1" };
static const uint8_t DCI_IEC61850_SAV_SMP_SYNC1_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES1_DEFAULT_ATTRIB = 8U;
static const uint8_t DCI_IEC61850_SAV_VLAN_PRI2_DEFAULT_ATTRIB = 4U;
static const uint8_t DCI_IEC61850_SAV_MAC_ADD_ID2_DEFAULT_ATTRIB[6] = 
        { 0x01, 0xFE, 0xED, 0xBE, 0xEF, 0x21 };
static const uint8_t DCI_IEC61850_SAV_ID2_DEFAULT_ATTRIB[16] = 
        { "MU02" };
static const uint8_t DCI_IEC61850_SAV_CB_REF_NAME2_DEFAULT_ATTRIB[32] = 
        { "Device_2/LLN0.svbRef_1" };
static const uint8_t DCI_IEC61850_SAV_DATA_SET_NAME2_DEFAULT_ATTRIB[16] = 
        { "PhsMeas2" };
static const uint8_t DCI_IEC61850_SAV_CONFIG_REV2_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_IEC61850_SAV_NUM_ASDU2_DEFAULT_ATTRIB = 8U;
static const uint8_t DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES2_DEFAULT_ATTRIB = 8U;
static const uint8_t DCI_CLIENT_AP_SSID_DEFAULT_ATTRIB[32] = 
        { "Temp" };
static const uint8_t DCI_CLIENT_AP_PASSPHRASE_DEFAULT_ATTRIB[32] = 
        { "key12345" };
static const uint8_t DCI_CLIENT_AP_CHANNEL_DEFAULT_ATTRIB = 6U;
static const uint8_t DCI_CLIENT_AP_BAND_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_CLIENT_AP_RETRY_COUNT_DEFAULT_ATTRIB = 10U;
static const uint8_t DCI_SOURCE_AP_SSID_DEFAULT_ATTRIB[32] = 
        { "Temp" };
static const uint8_t DCI_SOURCE_AP_PASSPHRASE_DEFAULT_ATTRIB[33] = 
        { "key12345" };
static const uint8_t DCI_SOURCE_AP_CHANNEL_DEFAULT_ATTRIB = 6U;
static const uint8_t DCI_SOURCE_AP_MAX_CONN_DEFAULT_ATTRIB = 4U;
static const uint8_t DCI_DATA_LOG_SPEC_VERSION_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_EVENT_LOG_SPEC_VERSION_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_AUDIT_POWER_LOG_SPEC_VERSION_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_AUDIT_USER_LOG_SPEC_VERSION_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_DEFAULT_ATTRIB = 1U;
static const uint8_t DCI_IOT_NetworkSSID_DEFAULT_ATTRIB[32] = 
        { "" };
static const uint8_t DCI_IOT_NetworkPassword_DEFAULT_ATTRIB[32] = 
        { "" };
static const uint8_t DCI_IOT_MacAddress_DEFAULT_ATTRIB[20] = 
        { "" };
static const uint8_t DCI_BUILD_TYPE_DEFAULT_ATTRIB[32] = 
        { "" };
static const uint8_t DCI_NEW_PROG_PART_NUMBER_DEFAULT_ATTRIB[32] = 
        { "" };
static const uint8_t DCI_IOT_UNICAST_PRIMARY_UDP_KEY_DEFAULT_ATTRIB[48] = 
        { "" };
static const uint8_t DCI_IOT_GET_UNICAST_UDP_KEY_DEFAULT_ATTRIB[32] = 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const uint8_t DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_DEFAULT_ATTRIB[48] = 
        { "" };
static const uint8_t DCI_IOT_GET_BROADCAST_UDP_KEY_DEFAULT_ATTRIB[32] = 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const uint8_t DCI_IOT_UNICAST_SECONDARY_UDP_KEY_DEFAULT_ATTRIB[48] = 
        { "" };
static const uint8_t DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_DEFAULT_ATTRIB[32] = 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const uint8_t DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_DEFAULT_ATTRIB[48] = 
        { "" };
static const uint8_t DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_DEFAULT_ATTRIB[32] = 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const uint8_t DCI_IOT_ErrorLogData_DEFAULT_ATTRIB[100] = 
        { "" };
static const uint8_t DCI_IOT_PATH_STATUS_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IOT_PRIMARY_STATE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IOT_SECONDARY_STATE_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IOT_IDENTIFY_ME_DEFAULT_ATTRIB = 0U;
static const uint8_t DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_DEFAULT_ATTRIB = 0U;
static const int8_t DCI_BLE_TEST_SETTING_2_DEFAULT_ATTRIB = -15;
static const int8_t DCI_BLE_TX_POWER_DEFAULT_ATTRIB = -4;
static const dci_bool_t DCI_PHY1_FULL_DUPLEX_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_PHY1_AUTONEG_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_PHY1_PORT_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_PHY2_FULL_DUPLEX_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_PHY2_AUTONEG_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_PHY2_PORT_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_ETH_ACD_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_HTTP_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_DEVICE_PARAMETERIZATION_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_DEFAULT_ATTRIB = False;
static const dci_bool_t DCI_DIGITAL_OUTPUT_BIT_DEFAULT_ATTRIB = False;
static const dci_bool_t DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_TRUSTED_IP_BACNET_IP_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_TRUSTED_IP_EIP_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_IOT_CONNECT_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_IOT_STATUS_DEFAULT_ATTRIB = False;
static const dci_bool_t DCI_IOT_PROXY_ENABLE_DEFAULT_ATTRIB = False;
static const dci_bool_t DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_DEFAULT_ATTRIB = False;
static const dci_bool_t DCI_BLE_TEST_SETTING_4_DEFAULT_ATTRIB = False;
static const dci_bool_t DCI_MODBUS_TCP_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_BACNET_IP_ENABLE_DEFAULT_ATTRIB = True;
static const dci_bool_t DCI_ENABLE_SESSION_TAKEOVER_DEFAULT_ATTRIB = 1U;
static const dci_bool_t DCI_MDNS_LWM2M_SERVICE_ENABLE_DEFAULT_ATTRIB = False;
static const dci_bool_t DCI_PTP_ENABLE_DEFAULT_ATTRIB = False;
static const dci_bool_t DCI_IOT_SECONDARY_CONTACT_STATUS_DEFAULT_ATTRIB = 0U;
static const dci_bool_t DCI_IOT_PRIMARY_CONTACT_STATUS_DEFAULT_ATTRIB = 0U;
static const dci_bool_t DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_DEFAULT_ATTRIB = 0U;
static const dci_bool_t DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_DEFAULT_ATTRIB = 0U;
static const dci_bool_t DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_DEFAULT_ATTRIB = 0U;
static const dci_bool_t DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_DEFAULT_ATTRIB = 0U;
static const dci_bool_t DCI_IOT_SBLCP_EN_DIS_DEFAULT_ATTRIB = 0U;


//*******************************************************
//******     Length Data Definitions
//*******************************************************

static const DCI_LENGTH_TD  DCI_PRODUCT_SERIAL_NUM_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_VENDOR_NAME_LENGTH = 12U;
static const DCI_LENGTH_TD  DCI_PRODUCT_CODE_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_PRODUCT_CODE_ASCII_LENGTH = 7U;
static const DCI_LENGTH_TD  DCI_VENDOR_URL_LENGTH = 14U;
static const DCI_LENGTH_TD  DCI_PRODUCT_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_MODEL_NAME_LENGTH = 10U;
static const DCI_LENGTH_TD  DCI_USER_APP_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_FW_UPGRADE_MODE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_APP_FIRM_VER_NUM_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_APP_FIRM_UPGRADE_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_LAST_APP_FIRM_UPGRADE_USER_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_LAST_WEB_FIRM_UPGRADE_USER_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_WEB_FIRM_VER_NUM_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_WEB_FIRM_UPGRADE_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_HARDWARE_VER_ASCII_LENGTH = 14U;
static const DCI_LENGTH_TD  DCI_HARDWARE_ID_VAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_CSTACK_USAGE_PERCENT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_FIRMWARE_CRC_VAL_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_HEAP_USED_MAX_PERC_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_HEAP_USED_PERC_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_RESET_CAUSE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_RESET_COUNTER_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IDLE_LOOP_TIME_AVE_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IDLE_LOOP_TIME_MAX_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IDLE_LOOP_TIME_MIN_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_ETHERNET_MAC_ADDRESS_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IP_ADDRESS_ALLOCATION_METHOD_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_ACTIVE_IP_ADDRESS_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_ACTIVE_SUBNET_MASK_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_ACTIVE_DEFAULT_GATEWAY_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_STATIC_IP_ADDRESS_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_STATIC_SUBNET_MASK_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_STATIC_DEFAULT_GATEWAY_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_PWR_BRD_DIP_SWITCHES_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_PHY1_LINK_SPEED_SELECT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_PHY1_LINK_SPEED_ACTUAL_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_PHY1_FULL_DUPLEX_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY1_FULL_DUPLEX_ACTUAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY1_AUTONEG_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY1_AUTONEG_STATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY1_PORT_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY2_LINK_SPEED_SELECT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_PHY2_LINK_SPEED_ACTUAL_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_PHY2_FULL_DUPLEX_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY2_FULL_DUPLEX_ACTUAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY2_AUTONEG_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY2_AUTONEG_STATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PHY2_PORT_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_MULTICAST_ENABLE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BROADCAST_ENABLE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_MODBUS_TCP_COMM_TIMEOUT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_MODBUS_SERIAL_COMM_TIMEOUT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_ETH_ACD_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_ETH_ACD_CONFLICT_STATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_ETH_ACD_CONFLICTED_STATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_ETH_ACD_CONFLICTED_MAC_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_ETH_ACD_CONFLICTED_ARP_PDU_LENGTH = 28U;
static const DCI_LENGTH_TD  DCI_SIMPLE_APP_CONTROL_WORD_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_SIMPLE_APP_STATUS_WORD_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_0_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_3_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_4_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_5_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_6_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_7_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_8_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_ASSY_9_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_0_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_1_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_2_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_3_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_4_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_5_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_6_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_7_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_8_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_MEMBERS_ASSY_9_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_LTK_VER_ASCII_LENGTH = 19U;
static const DCI_LENGTH_TD  DCI_EXAMPLE_PWM_DUTYCYCLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_EXAMPLE_PWM_FREQUENCY_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_EXAMPLE_MODBUS_SERIAL_BAUD_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_EXAMPLE_MODBUS_SERIAL_PARITY_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_EXAMPLE_ADC_VAL_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_RTC_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RTC_DATE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_WEB_FW_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_PRODUCT_PROC_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_PRODUCT_PROC_SERIAL_NUM_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_VALID_BITS_USERS_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_USER_1_FULLNAME_LENGTH = 33U;
static const DCI_LENGTH_TD  DCI_USER_1_USERNAME_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_1_PWD_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_1_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_1_PWD_SET_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_1_LAST_LOGIN_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_1_PWD_COMPLEXITY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_1_PWD_AGING_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_USER_2_FULLNAME_LENGTH = 33U;
static const DCI_LENGTH_TD  DCI_USER_2_USERNAME_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_2_PWD_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_2_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_2_PWD_SET_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_2_LAST_LOGIN_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_2_PWD_COMPLEXITY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_2_PWD_AGING_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_USER_3_FULLNAME_LENGTH = 33U;
static const DCI_LENGTH_TD  DCI_USER_3_USERNAME_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_3_PWD_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_3_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_3_PWD_SET_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_3_LAST_LOGIN_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_3_PWD_COMPLEXITY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_3_PWD_AGING_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_USER_4_FULLNAME_LENGTH = 33U;
static const DCI_LENGTH_TD  DCI_USER_4_USERNAME_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_4_PWD_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_4_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_4_PWD_SET_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_4_LAST_LOGIN_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_4_PWD_COMPLEXITY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_4_PWD_AGING_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_USER_5_FULLNAME_LENGTH = 33U;
static const DCI_LENGTH_TD  DCI_USER_5_USERNAME_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_5_PWD_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_5_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_5_PWD_SET_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_5_LAST_LOGIN_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_5_PWD_COMPLEXITY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_5_PWD_AGING_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_USER_6_FULLNAME_LENGTH = 33U;
static const DCI_LENGTH_TD  DCI_USER_6_USERNAME_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_6_PWD_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_USER_6_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_6_PWD_SET_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_6_LAST_LOGIN_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_6_PWD_COMPLEXITY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_6_PWD_AGING_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_HTTP_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IP_BIT_NOT_CONNECTED_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IP_WORD_NOT_CONNECTED_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IP_DWORD_NOT_CONNECTED_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_OP_BIT_NOT_CONNECTED_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_OP_WORD_NOT_CONNECTED_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_OP_DWORD_NOT_CONNECTED_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DEVICE_PARAMETERIZATION_ENABLE_LENGTH = 1U;
DCI_LENGTH_TD  DCI_PROFIBUS_PROD_MOD_DATA_LENGTH = 200U;
static const DCI_LENGTH_TD  DCI_STATUS_WARNING_BITS_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_PRODUCT_ACTIVE_FAULT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_SAMPLE_APP_INPUT_BIT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SAMPLE_APP_INPUT_WORD_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_SAMPLE_APP_INPUT_DWORD_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_SAMPLE_APP_OUTPUT_BIT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SAMPLE_APP_OUTPUT_WORD_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_SAMPLE_APP_OUTPUT_DWORD_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SAMPLE_APP_PARAMETER_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SAMPLE_ACYCLIC_PARAMETER_1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_SAMPLE_ACYCLIC_PARAMETER_2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_SAMPLE_ACYCLIC_PARAMETER_3_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SAMPLE_ACYCLIC_PARAMETER_4_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SAMPLE_ACYCLIC_PARAMETER_5_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_SAMPLE_ACYCLIC_PARAMETER_6_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_INPUT_TEST_BIT_1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_OUTPUT_TEST_BIT_1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_CIP_VENDOR_ID_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_CIP_DEVICE_TYPE_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_CIP_DEVICE_IDENT_DEV_STATUS_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_ETH_NAME_SERVER_1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_ETH_NAME_SERVER_2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_ETH_DOMAIN_NAME_LENGTH = 48U;
static const DCI_LENGTH_TD  DCI_ETH_HOST_NAME_LENGTH = 64U;
static const DCI_LENGTH_TD  DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_APP_FIRM_VER_LENGTH = 2U;
DCI_LENGTH_TD  DCI_EIP_TEST_INPUT_ASM_LENGTH = 2U;
DCI_LENGTH_TD  DCI_EIP_TEST_OUTPUT_ASM_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_CIP_DEVICE_IDENT_STATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_TERMINATOR_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_MEMBER_1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_MEMBER_2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_MEMBER_3_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_MEMBER_4_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_MEMBER_5_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_MEMBER_6_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_MEMBER_7_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_MEMBER_8_LENGTH = 4U;
DCI_LENGTH_TD  DCI_DYN_IN_ASM_INTERFACE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_SELECT_1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_SELECT_2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_SELECT_3_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_SELECT_4_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_SELECT_5_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_SELECT_6_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_SELECT_7_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_IN_ASM_SELECT_8_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_TERMINATOR_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_MEMBER_1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_MEMBER_2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_MEMBER_3_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_MEMBER_4_LENGTH = 4U;
DCI_LENGTH_TD  DCI_DYN_OUT_ASM_INTERFACE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_SELECT_1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_SELECT_2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_SELECT_3_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DYN_OUT_ASM_SELECT_4_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LTK_FAULT_QUEUE_SORTED_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_LTK_FAULT_QUEUE_EVENT_ORDER_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_LTK_FAULT_STATUS_BITS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_FAULT_RESET_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_EIP_TEST1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_EIP_TEST2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_EIP_TEST3_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_EIP_TEST4_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BACNET_VENDOR_IDENTIFIER_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_MAX_MASTER_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_MAX_INFO_FRAMES_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PROFILE_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_UPD_PORT_NUM_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_FOREIGN_DEVICE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_BBMD_IP_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_BBMD_PORT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_REGISTRATION_INTERVAL_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_COMM_TIMEOUT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_PROTOCOL_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_FAULT_BEHAVIOR_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_ENABLE_BACNET_IP_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BACNET_DEVICE_ADDR_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_INSTANCE_NUM_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BACNET_MSTP_INSTANCE_NUM_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BACNET_MSTP_COMM_TIMEOUT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BACNET_MSTP_BAUD_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BACNET_MSTP_PARITY_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BACNET_MSTP_STOPBIT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BACNET_MSTP_PROTOCOL_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BACNET_FAULT_CODE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BACNET_DATABASE_REVISION_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_ANALOG_VALUE_0_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_ANALOG_VALUE_1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_AV_ANY_PARAM_NUM_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_AV_ANY_PARAM_VAL_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_ANALOG_INPUT_0_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_ANALOG_INPUT_1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DIGITAL_INPUT_WORD_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DIGITAL_OUTPUT_WORD_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DIGITAL_INPUT_BIT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_DIGITAL_OUTPUT_BIT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_MULTI_STATE_0_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_MULTI_STATE_1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_TRUSTED_IP_WHITELIST_LENGTH = 12U;
static const DCI_LENGTH_TD  DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_TRUSTED_IP_BACNET_IP_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_TRUSTED_IP_EIP_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_UNIX_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_UNIX_EPOCH_TIME_64_BIT_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_TIME_OFFSET_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DATE_FORMAT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_TIME_FORMAT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_REST_RESET_COMMAND_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_DEVICE_CERT_VALID_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_DEVICE_CERT_CONTROL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_FAULT_CATCHER_LENGTH = 88U;
static const DCI_LENGTH_TD  DCI_SNTP_SERVICE_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SNTP_SERVER_1_LENGTH = 40U;
static const DCI_LENGTH_TD  DCI_SNTP_SERVER_2_LENGTH = 40U;
static const DCI_LENGTH_TD  DCI_SNTP_SERVER_3_LENGTH = 40U;
static const DCI_LENGTH_TD  DCI_SNTP_UPDATE_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_SNTP_RETRY_TIME_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_SNTP_ACTIVE_SERVER_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_CONNECT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_PROXY_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_PROXY_SERVER_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_IOT_PROXY_PORT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IOT_PROXY_USERNAME_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_IOT_PROXY_PASSWORD_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_IOT_DEVICE_GUID_LENGTH = 37U;
static const DCI_LENGTH_TD  DCI_IOT_DEVICE_PROFILE_LENGTH = 37U;
static const DCI_LENGTH_TD  DCI_IOT_CONN_STRING_LENGTH = 170U;
static const DCI_LENGTH_TD  DCI_IOT_UPDATE_RATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_CADENCE_UPDATE_RATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_GROUP0_CADENCE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_GROUP1_CADENCE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_GROUP2_CADENCE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_GROUP3_CADENCE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_CONN_STAT_REASON_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LOG_INTERVAL_TIME_MS_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_CHANGE_BASED_LOGGING_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_MANUAL_LOG_TRIGGER_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LOG_TEST_1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_SNAME_LENGTH = 13U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_PROFILE_ID_LENGTH = 17U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_VENDOR_LENGTH = 13U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_FAMILY_LENGTH = 7U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_ROLE_LENGTH = 11U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_MODEL_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_LNAME_LENGTH = 18U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_HW_VER_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_SW_VER_LENGTH = 10U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_USERNAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_PASSWORD_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_MANUFACTURER_NAME_LENGTH = 11U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_SERIAL_NO_LENGTH = 10U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_IMAGE_URL_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_BONDING_INFO_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_MIN_VAL_CONN_INTERVAL_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_MAX_VAL_CONN_INTERVAL_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_SLAVE_LATENCY_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_SUPERVISION_TIMEOUT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_ACTIVE_ACCESS_LEVEL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_DEVICE_PASSKEY_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_3_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_A_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_B_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_C_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_D_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_E_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_F_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_READING_1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_4_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_READING_2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_READING_3_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_5_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_6_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_SETTING_7_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_PRESENT_STATUS_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_DEVICE_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_EVENT_LIST_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_CR_PRIORITY_TIMING_LENGTH = 24U;
static const DCI_LENGTH_TD  DCI_OS_PRIORITY_TIMING_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_CPU_UTILIZATION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LOGGING_EVENT_CODE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_DEVICE_STATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LOG_EVENT_TEST_101_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LOG_EVENT_TEST_102_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_NV_MEM_ID_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_NV_PARAMETER_ADDRESS_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_NV_FAIL_OPERATION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LOG_PROC_IMAGE_ID_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LOG_FIRMWARE_VERSION_LENGTH = 14U;
static const DCI_LENGTH_TD  DCI_LOGGED_USERNAME_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_LOG_IP_ADDRESS_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_MODBUS_TCP_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BACNET_IP_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LOG_PORT_NUMBER_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_ADMIN_PASSWORD_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_OBSERVER_PASSWORD_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_OPERATOR_PASSWORD_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_ENGINEER_PASSWORD_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_USER1_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_USER1_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_USER1_PASSWORD_LENGTH = 28U;
static const DCI_LENGTH_TD  DCI_BLE_USER2_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_USER2_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_USER2_PASSWORD_LENGTH = 28U;
static const DCI_LENGTH_TD  DCI_BLE_USER3_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_USER3_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_BLE_USER4_ROLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_USER4_NAME_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_RESET_LINE_NUMBER_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RESET_FUNCTION_NAME_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_RESET_OS_TASK_NAME_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_RESET_OS_TASK_PRIORITY_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RESET_OS_STACK_SIZE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RESET_CR_TASK_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RESET_CR_TASK_PARAM_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RESET_CSTACK_SIZE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RESET_HIGHEST_OS_STACK_SIZE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_RESET_HEAP_USAGE_PERCENT_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_RESET_SOURCE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_ENABLE_SESSION_TAKEOVER_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_MAX_CONCURRENT_SESSION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_USER_INACTIVITY_TIMEOUT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_ABSOLUTE_TIMEOUT_SEC_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_USER_LOCK_TIME_SEC_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_MAX_FAILED_LOGIN_ATTEMPTS_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_CORS_TYPE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_EDDYSTONE_URL_LENGTH = 18U;
static const DCI_LENGTH_TD  DCI_BLE_TX_POWER_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_BLE_IBEACON_MAJOR_NUMBER_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_IBEACON_MINOR_NUMBER_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_BLE_IBEACON_128_BIT_UUID_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_MDNS_LWM2M_SERVICE_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_MDNS_SERVER1_NAME_LENGTH = 64U;
static const DCI_LENGTH_TD  DCI_MDNS_SERVER1_IP_ADDR_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_MDNS_SERVER1_PORT_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX3_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX4_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX5_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX6_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX7_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX8_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX9_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_J1939_MSG_EX10_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_DEVICE_PWD_SETTING_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_LICENSE_INFORMATION_LENGTH = 84U;
static const DCI_LENGTH_TD  DCI_DEVICE_DEFAULT_PWD_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_LANG_PREF_SETTING_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_COMMON_LANG_PREF_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_LANG_FIRM_VER_NUM_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_LAST_LANG_FIRM_UPGRADE_USER_LENGTH = 21U;
static const DCI_LENGTH_TD  DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_LOG_LENGTH = 130U;
static const DCI_LENGTH_TD  DCI_BLE_LOG_INDEX_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BLE_TEST_COUNTER_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DATA_LOG_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_EVENT_LOG_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_AUDIT_POWER_LOG_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_AUDIT_FW_UPGRADE_LOG_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_AUDIT_USER_LOG_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_AUDIT_CONFIG_LOG_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_DEVICE_UP_TIME_SEC_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_PTP_ENABLE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_PTP_SECONDS_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_PTP_NANOSECONDS_TIME_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_PRI1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_VID1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_APP_ID1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_MAC_ADD_ID1_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_IEC61850_GOOSE_ID1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_CB_REF_NAME1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_DATA_SET_NAME1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_CONFIG_REV1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IEC61850_NDS_COM1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_TEST1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_NUM_DATA_SET_ENTRIES1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND1_STVAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND1_Q_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND1_T_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND2_STVAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND2_Q_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND2_T_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_PRI2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_VID2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_APP_ID2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_MAC_ADD_ID2_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_IEC61850_GOOSE_ID2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_CB_REF_NAME2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_DATA_SET_NAME2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_CONFIG_REV2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IEC61850_NDS_COM2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_TEST2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_NUM_DATA_SET_ENTRIES2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND3_STVAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND3_Q_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND3_T_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND4_STVAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND4_Q_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_IND4_T_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_PRI4_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_VID4_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_APP_ID4_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_MAC_ADD_ID4_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_IEC61850_GOOSE_ID4_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_CB_REF_NAME4_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_DATA_SET_NAME4_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_CONFIG_REV4_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IEC61850_NDS_COM4_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_TEST4_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_NUM_DATA_SET_ENTRIES4_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM1_STVAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM1_Q_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM1_T_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM2_STVAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM2_Q_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM2_T_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_PRI5_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_VID5_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_VLAN_APP_ID5_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_MAC_ADD_ID5_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_IEC61850_GOOSE_ID5_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_CB_REF_NAME5_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_DATA_SET_NAME5_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_CONFIG_REV5_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IEC61850_NDS_COM5_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_TEST5_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_NUM_DATA_SET_ENTRIES5_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM5_STVAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM5_Q_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM5_T_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM6_STVAL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM6_Q_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_ALM6_T_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_VLAN_PRI1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_VLAN_VID1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_VLAN_APP_ID1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_MAC_ADD_ID1_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_ID1_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_CB_REF_NAME1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_DATA_SET_NAME1_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_CONFIG_REV1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_SMP_SYNC1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_SMP_RATE1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_SMP_MODE1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_NUM_ASDU1_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES1_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHA_AMP1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHA_AMP1_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHB_AMP1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHB_AMP1_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHC_AMP1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHC_AMP1_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHN_AMP1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHN_AMP1_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHA_VOLT1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHA_VOLT1_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHB_VOLT1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHB_VOLT1_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHC_VOLT1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHC_VOLT1_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHN_VOLT1_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHN_VOLT1_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_VLAN_PRI2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_VLAN_VID2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_VLAN_APP_ID2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_MAC_ADD_ID2_LENGTH = 6U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_ID2_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_CB_REF_NAME2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_DATA_SET_NAME2_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_CONFIG_REV2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_SMP_SYNC2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_SMP_RATE2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_SMP_MODE2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC2_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_NUM_ASDU2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES2_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHA_AMP2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHA_AMP2_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHB_AMP2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHB_AMP2_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHC_AMP2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHC_AMP2_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHN_AMP2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHN_AMP2_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHA_VOLT2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHA_VOLT2_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHB_VOLT2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHB_VOLT2_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHC_VOLT2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHC_VOLT2_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHN_VOLT2_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IEC61850_SAV_PHN_VOLT2_Q_LENGTH = 16U;
static const DCI_LENGTH_TD  DCI_CLIENT_AP_SSID_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_CLIENT_AP_PASSPHRASE_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_CLIENT_AP_SECURITY_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_CLIENT_AP_CHANNEL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_CLIENT_AP_BAND_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_CLIENT_AP_RETRY_COUNT_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SOURCE_AP_SSID_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_SOURCE_AP_PASSPHRASE_LENGTH = 33U;
static const DCI_LENGTH_TD  DCI_SOURCE_AP_CHANNEL_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SOURCE_AP_MAX_CONN_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_SNTP_TIME_OFFSET_DEBUG_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_DATA_LOG_SPEC_VERSION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_EVENT_LOG_SPEC_VERSION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_AUDIT_POWER_LOG_SPEC_VERSION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_AUDIT_USER_LOG_SPEC_VERSION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_NV_MAP_VER_NUM_LENGTH = 2U;
static const DCI_LENGTH_TD  DCI_IOT_SECONDARY_CONTACT_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_NetworkSSID_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IOT_NetworkPassword_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IOT_FirmwareVersion_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_MacAddress_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_IOT_ResetBreaker_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_BUILD_TYPE_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_NEW_PROG_PART_NUMBER_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IOT_PRIMARY_CONTACT_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_VoltageL1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_VoltageL2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_CurrentL1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_CurrentL2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_ActivePowerL1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_ActivePowerL2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_ReactivePowerL1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_ReactivePowerL2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_ApparentPowerL1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_ApparentPowerL2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_ActiveEnergyL1_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ActiveEnergyL2_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ReactiveEnergyL1_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ReactiveEnergyL2_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ApparentEnergyL1_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ApparentEnergyL2_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_PowerFactorL1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_PowerFactorL2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_FrequencyL1_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_FrequencyL2_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_ReverseActiveEnergyL1_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ReverseReactiveEnergyL1_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ReverseApparentEnergyL1_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ReverseActiveEnergyL2_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ReverseReactiveEnergyL2_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_ReverseApparentEnergyL2_LENGTH = 8U;
static const DCI_LENGTH_TD  DCI_IOT_UNICAST_PRIMARY_UDP_KEY_LENGTH = 48U;
static const DCI_LENGTH_TD  DCI_IOT_GET_UNICAST_UDP_KEY_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_LENGTH = 48U;
static const DCI_LENGTH_TD  DCI_IOT_GET_BROADCAST_UDP_KEY_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IOT_UNICAST_SECONDARY_UDP_KEY_LENGTH = 48U;
static const DCI_LENGTH_TD  DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_LENGTH = 48U;
static const DCI_LENGTH_TD  DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_LENGTH = 32U;
static const DCI_LENGTH_TD  DCI_IOT_ErrorLogData_LENGTH = 100U;
static const DCI_LENGTH_TD  DCI_IOT_PATH_STATUS_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_PRIMARY_STATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_SECONDARY_STATE_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_Load_Percentage_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_DPS_ENDPOINT_LENGTH = 80U;
static const DCI_LENGTH_TD  DCI_IOT_DPS_ID_SCOPE_LENGTH = 20U;
static const DCI_LENGTH_TD  DCI_IOT_DPS_SYMM_KEY_LENGTH = 45U;
static const DCI_LENGTH_TD  DCI_IOT_DPS_DEVICE_REG_ID_LENGTH = 37U;
static const DCI_LENGTH_TD  DCI_IOT_IDENTIFY_ME_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_LENGTH = 1U;
static const DCI_LENGTH_TD  DCI_IOT_TEMPERATURE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_POWER_UP_STATE_LENGTH = 4U;
static const DCI_LENGTH_TD  DCI_IOT_SBLCP_EN_DIS_LENGTH = 1U;


//*******************************************************
//******     Create the Mother Data Block Structure
//*******************************************************
const DCI_DATA_BLOCK_TD dci_data_blocks[DCI_TOTAL_DCIDS] = 
{
    {        // Device Product Serial Number
        DCI_PRODUCT_SERIAL_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PRODUCT_SERIAL_NUM_RAM ),
        &DCI_PRODUCT_SERIAL_NUM_LENGTH,
        nullptr,
        DCI_PRODUCT_SERIAL_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK )
        ),
        DCI_PRODUCT_SERIAL_NUM_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Product Vendor Name
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VENDOR_NAME_DEFAULT_ATTRIB )),
        &DCI_VENDOR_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VENDOR_NAME_DEFAULT_ATTRIB ),
        DCI_VENDOR_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Product Code
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PRODUCT_CODE_DEFAULT_ATTRIB )),
        &DCI_PRODUCT_CODE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PRODUCT_CODE_DEFAULT_ATTRIB ),
        DCI_PRODUCT_CODE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Product Code in ASCII
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_PRODUCT_CODE_ASCII_DEFAULT_ATTRIB )),
        &DCI_PRODUCT_CODE_ASCII_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_PRODUCT_CODE_ASCII_DEFAULT_ATTRIB ),
        DCI_PRODUCT_CODE_ASCII_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Corporate URL
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VENDOR_URL_DEFAULT_ATTRIB )),
        &DCI_VENDOR_URL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VENDOR_URL_DEFAULT_ATTRIB ),
        DCI_VENDOR_URL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Product Name - Short description of Prod Code
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_PRODUCT_NAME_RAM ),
        &DCI_PRODUCT_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_PRODUCT_NAME_DEFAULT_ATTRIB ),
        DCI_PRODUCT_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_PRODUCT_NAME_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Model Name
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MODEL_NAME_DEFAULT_ATTRIB )),
        &DCI_MODEL_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MODEL_NAME_DEFAULT_ATTRIB ),
        DCI_MODEL_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User Application Name or Device Tag
        DCI_USER_APP_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_APP_NAME_RAM ),
        &DCI_USER_APP_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_APP_NAME_DEFAULT_ATTRIB ),
        DCI_USER_APP_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_USER_APP_NAME_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Firmware Upgrade Mode
        DCI_FW_UPGRADE_MODE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_FW_UPGRADE_MODE_RAM ),
        &DCI_FW_UPGRADE_MODE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_FW_UPGRADE_MODE_DEFAULT_ATTRIB ),
        DCI_FW_UPGRADE_MODE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_FW_UPGRADE_MODE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Application Firmware Version Number
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_APP_FIRM_VER_NUM_RAM ),
        &DCI_APP_FIRM_VER_NUM_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_APP_FIRM_VER_NUM_DEFAULT_ATTRIB ),
        DCI_APP_FIRM_VER_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Application Firmware Upgrade Epoch Time
        DCI_APP_FIRM_UPGRADE_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_APP_FIRM_UPGRADE_EPOCH_TIME_RAM ),
        &DCI_APP_FIRM_UPGRADE_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_APP_FIRM_UPGRADE_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_APP_FIRM_UPGRADE_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Web Firmware Upgrade Epoch Time
        DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_RAM ),
        &DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User who upgraded last application firmware
        DCI_LAST_APP_FIRM_UPGRADE_USER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_LAST_APP_FIRM_UPGRADE_USER_RAM ),
        &DCI_LAST_APP_FIRM_UPGRADE_USER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_LAST_APP_FIRM_UPGRADE_USER_DEFAULT_ATTRIB ),
        DCI_LAST_APP_FIRM_UPGRADE_USER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User who upgraded last web firmware
        DCI_LAST_WEB_FIRM_UPGRADE_USER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_LAST_WEB_FIRM_UPGRADE_USER_RAM ),
        &DCI_LAST_WEB_FIRM_UPGRADE_USER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_LAST_WEB_FIRM_UPGRADE_USER_DEFAULT_ATTRIB ),
        DCI_LAST_WEB_FIRM_UPGRADE_USER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Web Firmware Version Number
        DCI_WEB_FIRM_VER_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_WEB_FIRM_VER_NUM_RAM ),
        &DCI_WEB_FIRM_VER_NUM_LENGTH,
        nullptr,
        DCI_WEB_FIRM_VER_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Web Firmware Upgrade Status
        DCI_WEB_FIRM_UPGRADE_STATUS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_WEB_FIRM_UPGRADE_STATUS_RAM ),
        &DCI_WEB_FIRM_UPGRADE_STATUS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_WEB_FIRM_UPGRADE_STATUS_DEFAULT_ATTRIB ),
        DCI_WEB_FIRM_UPGRADE_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Product Hardware Version
        DCI_HARDWARE_VER_ASCII_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_HARDWARE_VER_ASCII_RAM ),
        &DCI_HARDWARE_VER_ASCII_LENGTH,
        nullptr,
        DCI_HARDWARE_VER_ASCII_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK )
        ),
        DCI_HARDWARE_VER_ASCII_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Hardware ID Value
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_HARDWARE_ID_VAL_RAM ),
        &DCI_HARDWARE_ID_VAL_LENGTH,
        nullptr,
        DCI_HARDWARE_ID_VAL_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Cstack Usage
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CSTACK_USAGE_PERCENT_RAM ),
        &DCI_CSTACK_USAGE_PERCENT_LENGTH,
        nullptr,
        DCI_CSTACK_USAGE_PERCENT_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Firmware CRC
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_FIRMWARE_CRC_VAL_RAM ),
        &DCI_FIRMWARE_CRC_VAL_LENGTH,
        nullptr,
        DCI_FIRMWARE_CRC_VAL_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Max Heap Stack Usage Percent
        DCI_HEAP_USED_MAX_PERC_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_HEAP_USED_MAX_PERC_RAM ),
        &DCI_HEAP_USED_MAX_PERC_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_HEAP_USED_MAX_PERC_DEFAULT_ATTRIB ),
        DCI_HEAP_USED_MAX_PERC_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Product Health - Heap Usage
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_HEAP_USED_PERC_RAM ),
        &DCI_HEAP_USED_PERC_LENGTH,
        nullptr,
        DCI_HEAP_USED_PERC_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Reset Cause
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_CAUSE_RAM ),
        &DCI_RESET_CAUSE_LENGTH,
        nullptr,
        DCI_RESET_CAUSE_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // Reset Counter
        DCI_RESET_COUNTER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_COUNTER_RAM ),
        &DCI_RESET_COUNTER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_COUNTER_DEFAULT_ATTRIB ),
        DCI_RESET_COUNTER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Idle Loop Time Average
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IDLE_LOOP_TIME_AVE_RAM ),
        &DCI_IDLE_LOOP_TIME_AVE_LENGTH,
        nullptr,
        DCI_IDLE_LOOP_TIME_AVE_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Idle Loop Time Max
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IDLE_LOOP_TIME_MAX_RAM ),
        &DCI_IDLE_LOOP_TIME_MAX_LENGTH,
        nullptr,
        DCI_IDLE_LOOP_TIME_MAX_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Idle Loop Time Min
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IDLE_LOOP_TIME_MIN_RAM ),
        &DCI_IDLE_LOOP_TIME_MIN_LENGTH,
        nullptr,
        DCI_IDLE_LOOP_TIME_MIN_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Ethernet MAC Address
        DCI_ETHERNET_MAC_ADDRESS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_ETHERNET_MAC_ADDRESS_RAM ),
        &DCI_ETHERNET_MAC_ADDRESS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_ETHERNET_MAC_ADDRESS_DEFAULT_ATTRIB ),
        DCI_ETHERNET_MAC_ADDRESS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK )
        ),
        DCI_ETHERNET_MAC_ADDRESS_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Status of Control of IP Address Allocation 
        DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_RAM ),
        &DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_DEFAULT_ATTRIB ),
        DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Allows to set IP Address Allocation Method
        DCI_IP_ADDRESS_ALLOCATION_METHOD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IP_ADDRESS_ALLOCATION_METHOD_RAM ),
        &DCI_IP_ADDRESS_ALLOCATION_METHOD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IP_ADDRESS_ALLOCATION_METHOD_DEFAULT_ATTRIB ),
        DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        DCI_IP_ADDRESS_ALLOCATION_METHOD_CBACK_INDEX,        //Not Default
        DCI_IP_ADDRESS_ALLOCATION_METHOD_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Shows the present IP Address Allocation Method
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_RAM ),
        &DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_LENGTH,
        nullptr,
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_CBACK_INDEX,        //Not Default
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Active IP Address
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_ACTIVE_IP_ADDRESS_RAM ),
        &DCI_ACTIVE_IP_ADDRESS_LENGTH,
        nullptr,
        DCI_ACTIVE_IP_ADDRESS_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Active Subnet Mask
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_ACTIVE_SUBNET_MASK_RAM ),
        &DCI_ACTIVE_SUBNET_MASK_LENGTH,
        nullptr,
        DCI_ACTIVE_SUBNET_MASK_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Active Default Gateway
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_ACTIVE_DEFAULT_GATEWAY_RAM ),
        &DCI_ACTIVE_DEFAULT_GATEWAY_LENGTH,
        nullptr,
        DCI_ACTIVE_DEFAULT_GATEWAY_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Static IP Address
        DCI_STATIC_IP_ADDRESS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_STATIC_IP_ADDRESS_RAM ),
        &DCI_STATIC_IP_ADDRESS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_STATIC_IP_ADDRESS_DEFAULT_ATTRIB ),
        DCI_STATIC_IP_ADDRESS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Static IP Address
        DCI_STATIC_SUBNET_MASK_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_STATIC_SUBNET_MASK_RAM ),
        &DCI_STATIC_SUBNET_MASK_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_STATIC_SUBNET_MASK_DEFAULT_ATTRIB ),
        DCI_STATIC_SUBNET_MASK_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Static Default Gateway
        DCI_STATIC_DEFAULT_GATEWAY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_STATIC_DEFAULT_GATEWAY_RAM ),
        &DCI_STATIC_DEFAULT_GATEWAY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_STATIC_DEFAULT_GATEWAY_DEFAULT_ATTRIB ),
        DCI_STATIC_DEFAULT_GATEWAY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // BCM DIP Switches
        DCI_PWR_BRD_DIP_SWITCHES_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PWR_BRD_DIP_SWITCHES_RAM ),
        &DCI_PWR_BRD_DIP_SWITCHES_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PWR_BRD_DIP_SWITCHES_DEFAULT_ATTRIB ),
        DCI_PWR_BRD_DIP_SWITCHES_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Ethernet PHY 1 - Link Speed Select
        DCI_PHY1_LINK_SPEED_SELECT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY1_LINK_SPEED_SELECT_RAM ),
        &DCI_PHY1_LINK_SPEED_SELECT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PHY1_LINK_SPEED_SELECT_DEFAULT_ATTRIB ),
        DCI_PHY1_LINK_SPEED_SELECT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY1_LINK_SPEED_SELECT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Ethernet PHY 1 - Link Speed Actual
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY1_LINK_SPEED_ACTUAL_RAM ),
        &DCI_PHY1_LINK_SPEED_ACTUAL_LENGTH,
        nullptr,
        DCI_PHY1_LINK_SPEED_ACTUAL_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Ethernet PHY 1 - Duplex Select
        DCI_PHY1_FULL_DUPLEX_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY1_FULL_DUPLEX_ENABLE_RAM ),
        &DCI_PHY1_FULL_DUPLEX_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PHY1_FULL_DUPLEX_ENABLE_DEFAULT_ATTRIB ),
        DCI_PHY1_FULL_DUPLEX_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY1_FULL_DUPLEX_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Ethernet PHY 1 - Duplex Actual
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY1_FULL_DUPLEX_ACTUAL_RAM ),
        &DCI_PHY1_FULL_DUPLEX_ACTUAL_LENGTH,
        nullptr,
        DCI_PHY1_FULL_DUPLEX_ACTUAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY1_FULL_DUPLEX_ACTUAL_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Ethernet PHY 1 - Link Auto-Negotiate Enable
        DCI_PHY1_AUTONEG_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY1_AUTONEG_ENABLE_RAM ),
        &DCI_PHY1_AUTONEG_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PHY1_AUTONEG_ENABLE_DEFAULT_ATTRIB ),
        DCI_PHY1_AUTONEG_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY1_AUTONEG_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Ethernet PHY 1 - Link Auto-Negotiate State
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY1_AUTONEG_STATE_RAM ),
        &DCI_PHY1_AUTONEG_STATE_LENGTH,
        nullptr,
        DCI_PHY1_AUTONEG_STATE_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ethernet PHY 1 - Port Enable
        DCI_PHY1_PORT_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY1_PORT_ENABLE_RAM ),
        &DCI_PHY1_PORT_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PHY1_PORT_ENABLE_DEFAULT_ATTRIB ),
        DCI_PHY1_PORT_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY1_PORT_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Ethernet PHY 2 - Link Speed Select
        DCI_PHY2_LINK_SPEED_SELECT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY2_LINK_SPEED_SELECT_RAM ),
        &DCI_PHY2_LINK_SPEED_SELECT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PHY2_LINK_SPEED_SELECT_DEFAULT_ATTRIB ),
        DCI_PHY2_LINK_SPEED_SELECT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY2_LINK_SPEED_SELECT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Ethernet PHY 2 - Link Speed Actual
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY2_LINK_SPEED_ACTUAL_RAM ),
        &DCI_PHY2_LINK_SPEED_ACTUAL_LENGTH,
        nullptr,
        DCI_PHY2_LINK_SPEED_ACTUAL_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Ethernet PHY 2 - Duplex Select
        DCI_PHY2_FULL_DUPLEX_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY2_FULL_DUPLEX_ENABLE_RAM ),
        &DCI_PHY2_FULL_DUPLEX_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PHY2_FULL_DUPLEX_ENABLE_DEFAULT_ATTRIB ),
        DCI_PHY2_FULL_DUPLEX_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY2_FULL_DUPLEX_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Ethernet PHY 2 - Duplex Actual
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY2_FULL_DUPLEX_ACTUAL_RAM ),
        &DCI_PHY2_FULL_DUPLEX_ACTUAL_LENGTH,
        nullptr,
        DCI_PHY2_FULL_DUPLEX_ACTUAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY2_FULL_DUPLEX_ACTUAL_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Ethernet PHY 2 - Link Auto-Negotiate Enable
        DCI_PHY2_AUTONEG_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY2_AUTONEG_ENABLE_RAM ),
        &DCI_PHY2_AUTONEG_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PHY2_AUTONEG_ENABLE_DEFAULT_ATTRIB ),
        DCI_PHY2_AUTONEG_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY2_AUTONEG_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Ethernet PHY 2 - Link Auto-Negotiate State
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY2_AUTONEG_STATE_RAM ),
        &DCI_PHY2_AUTONEG_STATE_LENGTH,
        nullptr,
        DCI_PHY2_AUTONEG_STATE_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ethernet PHY 2 - Port Enable
        DCI_PHY2_PORT_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PHY2_PORT_ENABLE_RAM ),
        &DCI_PHY2_PORT_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PHY2_PORT_ENABLE_DEFAULT_ATTRIB ),
        DCI_PHY2_PORT_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PHY2_PORT_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Multicast settings
        DCI_MULTICAST_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MULTICAST_ENABLE_RAM ),
        &DCI_MULTICAST_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MULTICAST_ENABLE_DEFAULT_ATTRIB ),
        DCI_MULTICAST_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Broadcast settings
        DCI_BROADCAST_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BROADCAST_ENABLE_RAM ),
        &DCI_BROADCAST_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BROADCAST_ENABLE_DEFAULT_ATTRIB ),
        DCI_BROADCAST_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Modbus Comm Loss Timeout Value tcp(ms)
        DCI_MODBUS_TCP_COMM_TIMEOUT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MODBUS_TCP_COMM_TIMEOUT_RAM ),
        &DCI_MODBUS_TCP_COMM_TIMEOUT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MODBUS_TCP_COMM_TIMEOUT_DEFAULT_ATTRIB ),
        DCI_MODBUS_TCP_COMM_TIMEOUT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Modbus Comm Loss Timeout Value for serial(ms)
        DCI_MODBUS_SERIAL_COMM_TIMEOUT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MODBUS_SERIAL_COMM_TIMEOUT_RAM ),
        &DCI_MODBUS_SERIAL_COMM_TIMEOUT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MODBUS_SERIAL_COMM_TIMEOUT_DEFAULT_ATTRIB ),
        DCI_MODBUS_SERIAL_COMM_TIMEOUT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // ACD Enable
        DCI_ETH_ACD_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ETH_ACD_ENABLE_RAM ),
        &DCI_ETH_ACD_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ETH_ACD_ENABLE_DEFAULT_ATTRIB ),
        DCI_ETH_ACD_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_ETH_ACD_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // ACD Conflict State
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ETH_ACD_CONFLICT_STATE_RAM ),
        &DCI_ETH_ACD_CONFLICT_STATE_LENGTH,
        nullptr,
        DCI_ETH_ACD_CONFLICT_STATE_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // ACD Conflicted State
        DCI_ETH_ACD_CONFLICTED_STATE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ETH_ACD_CONFLICTED_STATE_RAM ),
        &DCI_ETH_ACD_CONFLICTED_STATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ETH_ACD_CONFLICTED_STATE_DEFAULT_ATTRIB ),
        DCI_ETH_ACD_CONFLICTED_STATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // ACD Conflicted Device MAC
        DCI_ETH_ACD_CONFLICTED_MAC_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_ETH_ACD_CONFLICTED_MAC_RAM ),
        &DCI_ETH_ACD_CONFLICTED_MAC_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_ETH_ACD_CONFLICTED_MAC_DEFAULT_ATTRIB ),
        DCI_ETH_ACD_CONFLICTED_MAC_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // ACD ARP PDU Conflicted Message
        DCI_ETH_ACD_CONFLICTED_ARP_PDU_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_ETH_ACD_CONFLICTED_ARP_PDU_RAM ),
        &DCI_ETH_ACD_CONFLICTED_ARP_PDU_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_ETH_ACD_CONFLICTED_ARP_PDU_DEFAULT_ATTRIB ),
        DCI_ETH_ACD_CONFLICTED_ARP_PDU_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Switch to control LED
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SIMPLE_APP_CONTROL_WORD_RAM ),
        &DCI_SIMPLE_APP_CONTROL_WORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SIMPLE_APP_CONTROL_WORD_DEFAULT_ATTRIB ),
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_SIMPLE_APP_CONTROL_WORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Switches to display the LED status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SIMPLE_APP_STATUS_WORD_RAM ),
        &DCI_SIMPLE_APP_STATUS_WORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SIMPLE_APP_STATUS_WORD_DEFAULT_ATTRIB ),
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Web Access Password Waiver Level
        DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_RAM ),
        &DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_DEFAULT_ATTRIB ),
        DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // User Assembly 0 Valid Bits
        DCI_VALID_BITS_ASSY_0_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_0_RAM ),
        &DCI_VALID_BITS_ASSY_0_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_0_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_0_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 1 Valid Bits
        DCI_VALID_BITS_ASSY_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_1_RAM ),
        &DCI_VALID_BITS_ASSY_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_1_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 2 Valid Bits
        DCI_VALID_BITS_ASSY_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_2_RAM ),
        &DCI_VALID_BITS_ASSY_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_2_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 3 Valid Bits
        DCI_VALID_BITS_ASSY_3_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_3_RAM ),
        &DCI_VALID_BITS_ASSY_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_3_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 4 Valid Bits
        DCI_VALID_BITS_ASSY_4_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_4_RAM ),
        &DCI_VALID_BITS_ASSY_4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_4_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 5 Valid Bits
        DCI_VALID_BITS_ASSY_5_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_5_RAM ),
        &DCI_VALID_BITS_ASSY_5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_5_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 6 Valid Bits
        DCI_VALID_BITS_ASSY_6_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_6_RAM ),
        &DCI_VALID_BITS_ASSY_6_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_6_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_6_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 7 Valid Bits
        DCI_VALID_BITS_ASSY_7_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_7_RAM ),
        &DCI_VALID_BITS_ASSY_7_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_7_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_7_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 8 Valid Bits
        DCI_VALID_BITS_ASSY_8_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_8_RAM ),
        &DCI_VALID_BITS_ASSY_8_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_8_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_8_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 9 Valid Bits
        DCI_VALID_BITS_ASSY_9_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_VALID_BITS_ASSY_9_RAM ),
        &DCI_VALID_BITS_ASSY_9_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_VALID_BITS_ASSY_9_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_ASSY_9_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // User Assembly 0 Members
        DCI_MEMBERS_ASSY_0_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_0_RAM ),
        &DCI_MEMBERS_ASSY_0_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_0_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_0_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 1 Members
        DCI_MEMBERS_ASSY_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_1_RAM ),
        &DCI_MEMBERS_ASSY_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_1_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 2 Members
        DCI_MEMBERS_ASSY_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_2_RAM ),
        &DCI_MEMBERS_ASSY_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_2_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 3 Members
        DCI_MEMBERS_ASSY_3_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_3_RAM ),
        &DCI_MEMBERS_ASSY_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_3_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 4 Members
        DCI_MEMBERS_ASSY_4_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_4_RAM ),
        &DCI_MEMBERS_ASSY_4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_4_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 5 Members
        DCI_MEMBERS_ASSY_5_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_5_RAM ),
        &DCI_MEMBERS_ASSY_5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_5_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 6 Members
        DCI_MEMBERS_ASSY_6_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_6_RAM ),
        &DCI_MEMBERS_ASSY_6_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_6_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_6_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 7 Members
        DCI_MEMBERS_ASSY_7_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_7_RAM ),
        &DCI_MEMBERS_ASSY_7_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_7_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_7_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 8 Members
        DCI_MEMBERS_ASSY_8_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_8_RAM ),
        &DCI_MEMBERS_ASSY_8_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_8_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_8_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User Assembly 9 Members
        DCI_MEMBERS_ASSY_9_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MEMBERS_ASSY_9_RAM ),
        &DCI_MEMBERS_ASSY_9_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MEMBERS_ASSY_9_DEFAULT_ATTRIB ),
        DCI_MEMBERS_ASSY_9_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // LTK Revision ASCII
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_LTK_VER_ASCII_DEFAULT_ATTRIB )),
        &DCI_LTK_VER_ASCII_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_LTK_VER_ASCII_DEFAULT_ATTRIB ),
        DCI_LTK_VER_ASCII_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // uC_PWM example code dutycycle
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EXAMPLE_PWM_DUTYCYCLE_RAM ),
        &DCI_EXAMPLE_PWM_DUTYCYCLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EXAMPLE_PWM_DUTYCYCLE_DEFAULT_ATTRIB ),
        DCI_EXAMPLE_PWM_DUTYCYCLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_EXAMPLE_PWM_DUTYCYCLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // uC_PWM example code frequency
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EXAMPLE_PWM_FREQUENCY_RAM ),
        &DCI_EXAMPLE_PWM_FREQUENCY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EXAMPLE_PWM_FREQUENCY_DEFAULT_ATTRIB ),
        DCI_EXAMPLE_PWM_FREQUENCY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Modbus serial example address
        DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_RAM ),
        &DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_DEFAULT_ATTRIB ),
        DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // modbus serial example baudrate
        DCI_EXAMPLE_MODBUS_SERIAL_BAUD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EXAMPLE_MODBUS_SERIAL_BAUD_RAM ),
        &DCI_EXAMPLE_MODBUS_SERIAL_BAUD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EXAMPLE_MODBUS_SERIAL_BAUD_DEFAULT_ATTRIB ),
        DCI_EXAMPLE_MODBUS_SERIAL_BAUD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // modbus serial example parity
        DCI_EXAMPLE_MODBUS_SERIAL_PARITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EXAMPLE_MODBUS_SERIAL_PARITY_RAM ),
        &DCI_EXAMPLE_MODBUS_SERIAL_PARITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EXAMPLE_MODBUS_SERIAL_PARITY_DEFAULT_ATTRIB ),
        DCI_EXAMPLE_MODBUS_SERIAL_PARITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_EXAMPLE_MODBUS_SERIAL_PARITY_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // modbus serial example stopbit
        DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_RAM ),
        &DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_DEFAULT_ATTRIB ),
        DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // uC_AtoD example code ADC value
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EXAMPLE_ADC_VAL_RAM ),
        &DCI_EXAMPLE_ADC_VAL_LENGTH,
        nullptr,
        DCI_EXAMPLE_ADC_VAL_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // RTC Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RTC_TIME_RAM ),
        &DCI_RTC_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RTC_TIME_DEFAULT_ATTRIB ),
        DCI_RTC_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // RTC Date
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RTC_DATE_RAM ),
        &DCI_RTC_DATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RTC_DATE_DEFAULT_ATTRIB ),
        DCI_RTC_DATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Product Web Firmware Name
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_WEB_FW_NAME_RAM ),
        &DCI_USER_WEB_FW_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_WEB_FW_NAME_DEFAULT_ATTRIB ),
        DCI_USER_WEB_FW_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Product proce Name - Short description of Prod Code
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_PRODUCT_PROC_NAME_RAM ),
        &DCI_PRODUCT_PROC_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_PRODUCT_PROC_NAME_DEFAULT_ATTRIB ),
        DCI_PRODUCT_PROC_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Device Product proc Serial Number
        DCI_PRODUCT_PROC_SERIAL_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PRODUCT_PROC_SERIAL_NUM_RAM ),
        &DCI_PRODUCT_PROC_SERIAL_NUM_LENGTH,
        nullptr,
        DCI_PRODUCT_PROC_SERIAL_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK )
        ),
        DCI_PRODUCT_PROC_SERIAL_NUM_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Active Users Valid bits
        DCI_VALID_BITS_USERS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_VALID_BITS_USERS_RAM ),
        &DCI_VALID_BITS_USERS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_VALID_BITS_USERS_DEFAULT_ATTRIB ),
        DCI_VALID_BITS_USERS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User 1 Full Name
        DCI_USER_1_FULLNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_1_FULLNAME_RAM ),
        &DCI_USER_1_FULLNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_1_FULLNAME_DEFAULT_ATTRIB ),
        DCI_USER_1_FULLNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 1 User Name
        DCI_USER_1_USERNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_1_USERNAME_RAM ),
        &DCI_USER_1_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_1_USERNAME_DEFAULT_ATTRIB ),
        DCI_USER_1_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 1 Password
        DCI_USER_1_PWD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_1_PWD_RAM ),
        &DCI_USER_1_PWD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_1_PWD_DEFAULT_ATTRIB ),
        DCI_USER_1_PWD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_USER_1_PWD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 1 Assigned Role
        DCI_USER_1_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_1_ROLE_RAM ),
        &DCI_USER_1_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_1_ROLE_DEFAULT_ATTRIB ),
        DCI_USER_1_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 1 Password Set Epoch Time
        DCI_USER_1_PWD_SET_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_1_PWD_SET_EPOCH_TIME_RAM ),
        &DCI_USER_1_PWD_SET_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_1_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_1_PWD_SET_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 1 Last Login Epoch Time
        DCI_USER_1_LAST_LOGIN_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_1_LAST_LOGIN_EPOCH_TIME_RAM ),
        &DCI_USER_1_LAST_LOGIN_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_1_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_1_LAST_LOGIN_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 1 Password Complexity setting
        DCI_USER_1_PWD_COMPLEXITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_1_PWD_COMPLEXITY_RAM ),
        &DCI_USER_1_PWD_COMPLEXITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_1_PWD_COMPLEXITY_DEFAULT_ATTRIB ),
        DCI_USER_1_PWD_COMPLEXITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 1 Password Aging setting
        DCI_USER_1_PWD_AGING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_1_PWD_AGING_RAM ),
        &DCI_USER_1_PWD_AGING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_1_PWD_AGING_DEFAULT_ATTRIB ),
        DCI_USER_1_PWD_AGING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User 2 Full Name
        DCI_USER_2_FULLNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_2_FULLNAME_RAM ),
        &DCI_USER_2_FULLNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_2_FULLNAME_DEFAULT_ATTRIB ),
        DCI_USER_2_FULLNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 2 User Name
        DCI_USER_2_USERNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_2_USERNAME_RAM ),
        &DCI_USER_2_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_2_USERNAME_DEFAULT_ATTRIB ),
        DCI_USER_2_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 2 Password
        DCI_USER_2_PWD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_2_PWD_RAM ),
        &DCI_USER_2_PWD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_2_PWD_DEFAULT_ATTRIB ),
        DCI_USER_2_PWD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_USER_2_PWD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 2 Assigned Role
        DCI_USER_2_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_2_ROLE_RAM ),
        &DCI_USER_2_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_2_ROLE_DEFAULT_ATTRIB ),
        DCI_USER_2_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 2 Password Set Epoch Time
        DCI_USER_2_PWD_SET_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_2_PWD_SET_EPOCH_TIME_RAM ),
        &DCI_USER_2_PWD_SET_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_2_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_2_PWD_SET_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 2 Last Login Epoch Time
        DCI_USER_2_LAST_LOGIN_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_2_LAST_LOGIN_EPOCH_TIME_RAM ),
        &DCI_USER_2_LAST_LOGIN_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_2_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_2_LAST_LOGIN_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 2 Password Complexity setting
        DCI_USER_2_PWD_COMPLEXITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_2_PWD_COMPLEXITY_RAM ),
        &DCI_USER_2_PWD_COMPLEXITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_2_PWD_COMPLEXITY_DEFAULT_ATTRIB ),
        DCI_USER_2_PWD_COMPLEXITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 2 Password Aging setting
        DCI_USER_2_PWD_AGING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_2_PWD_AGING_RAM ),
        &DCI_USER_2_PWD_AGING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_2_PWD_AGING_DEFAULT_ATTRIB ),
        DCI_USER_2_PWD_AGING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User 3 Full Name
        DCI_USER_3_FULLNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_3_FULLNAME_RAM ),
        &DCI_USER_3_FULLNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_3_FULLNAME_DEFAULT_ATTRIB ),
        DCI_USER_3_FULLNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 3 User Name
        DCI_USER_3_USERNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_3_USERNAME_RAM ),
        &DCI_USER_3_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_3_USERNAME_DEFAULT_ATTRIB ),
        DCI_USER_3_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 3 Password
        DCI_USER_3_PWD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_3_PWD_RAM ),
        &DCI_USER_3_PWD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_3_PWD_DEFAULT_ATTRIB ),
        DCI_USER_3_PWD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_USER_3_PWD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 3 Assigned Role
        DCI_USER_3_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_3_ROLE_RAM ),
        &DCI_USER_3_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_3_ROLE_DEFAULT_ATTRIB ),
        DCI_USER_3_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 3 Password Set Epoch Time
        DCI_USER_3_PWD_SET_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_3_PWD_SET_EPOCH_TIME_RAM ),
        &DCI_USER_3_PWD_SET_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_3_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_3_PWD_SET_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 3 Last Login Epoch Time
        DCI_USER_3_LAST_LOGIN_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_3_LAST_LOGIN_EPOCH_TIME_RAM ),
        &DCI_USER_3_LAST_LOGIN_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_3_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_3_LAST_LOGIN_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 3 Password Complexity setting
        DCI_USER_3_PWD_COMPLEXITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_3_PWD_COMPLEXITY_RAM ),
        &DCI_USER_3_PWD_COMPLEXITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_3_PWD_COMPLEXITY_DEFAULT_ATTRIB ),
        DCI_USER_3_PWD_COMPLEXITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 3 Password Aging setting
        DCI_USER_3_PWD_AGING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_3_PWD_AGING_RAM ),
        &DCI_USER_3_PWD_AGING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_3_PWD_AGING_DEFAULT_ATTRIB ),
        DCI_USER_3_PWD_AGING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User 4 Full Name
        DCI_USER_4_FULLNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_4_FULLNAME_RAM ),
        &DCI_USER_4_FULLNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_4_FULLNAME_DEFAULT_ATTRIB ),
        DCI_USER_4_FULLNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 4 User Name
        DCI_USER_4_USERNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_4_USERNAME_RAM ),
        &DCI_USER_4_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_4_USERNAME_DEFAULT_ATTRIB ),
        DCI_USER_4_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 4 Password
        DCI_USER_4_PWD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_4_PWD_RAM ),
        &DCI_USER_4_PWD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_4_PWD_DEFAULT_ATTRIB ),
        DCI_USER_4_PWD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_USER_4_PWD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 4 Assigned Role
        DCI_USER_4_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_4_ROLE_RAM ),
        &DCI_USER_4_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_4_ROLE_DEFAULT_ATTRIB ),
        DCI_USER_4_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 4 Password Set Epoch Time
        DCI_USER_4_PWD_SET_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_4_PWD_SET_EPOCH_TIME_RAM ),
        &DCI_USER_4_PWD_SET_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_4_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_4_PWD_SET_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 4 Last Login Epoch Time
        DCI_USER_4_LAST_LOGIN_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_4_LAST_LOGIN_EPOCH_TIME_RAM ),
        &DCI_USER_4_LAST_LOGIN_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_4_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_4_LAST_LOGIN_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 4 Password Complexity setting
        DCI_USER_4_PWD_COMPLEXITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_4_PWD_COMPLEXITY_RAM ),
        &DCI_USER_4_PWD_COMPLEXITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_4_PWD_COMPLEXITY_DEFAULT_ATTRIB ),
        DCI_USER_4_PWD_COMPLEXITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 4 Password Aging setting
        DCI_USER_4_PWD_AGING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_4_PWD_AGING_RAM ),
        &DCI_USER_4_PWD_AGING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_4_PWD_AGING_DEFAULT_ATTRIB ),
        DCI_USER_4_PWD_AGING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User 5 Full Name
        DCI_USER_5_FULLNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_5_FULLNAME_RAM ),
        &DCI_USER_5_FULLNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_5_FULLNAME_DEFAULT_ATTRIB ),
        DCI_USER_5_FULLNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 5 User Name
        DCI_USER_5_USERNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_5_USERNAME_RAM ),
        &DCI_USER_5_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_5_USERNAME_DEFAULT_ATTRIB ),
        DCI_USER_5_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 5 Password
        DCI_USER_5_PWD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_5_PWD_RAM ),
        &DCI_USER_5_PWD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_5_PWD_DEFAULT_ATTRIB ),
        DCI_USER_5_PWD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_USER_5_PWD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 5 Assigned Role
        DCI_USER_5_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_5_ROLE_RAM ),
        &DCI_USER_5_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_5_ROLE_DEFAULT_ATTRIB ),
        DCI_USER_5_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 5 Password Set Epoch Time
        DCI_USER_5_PWD_SET_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_5_PWD_SET_EPOCH_TIME_RAM ),
        &DCI_USER_5_PWD_SET_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_5_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_5_PWD_SET_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 5 Last Login Epoch Time
        DCI_USER_5_LAST_LOGIN_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_5_LAST_LOGIN_EPOCH_TIME_RAM ),
        &DCI_USER_5_LAST_LOGIN_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_5_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_5_LAST_LOGIN_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 5 Password Complexity setting
        DCI_USER_5_PWD_COMPLEXITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_5_PWD_COMPLEXITY_RAM ),
        &DCI_USER_5_PWD_COMPLEXITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_5_PWD_COMPLEXITY_DEFAULT_ATTRIB ),
        DCI_USER_5_PWD_COMPLEXITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 5 Password Aging setting
        DCI_USER_5_PWD_AGING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_5_PWD_AGING_RAM ),
        &DCI_USER_5_PWD_AGING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_5_PWD_AGING_DEFAULT_ATTRIB ),
        DCI_USER_5_PWD_AGING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // User 6 Full Name
        DCI_USER_6_FULLNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_6_FULLNAME_RAM ),
        &DCI_USER_6_FULLNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_6_FULLNAME_DEFAULT_ATTRIB ),
        DCI_USER_6_FULLNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 6 User Name
        DCI_USER_6_USERNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_6_USERNAME_RAM ),
        &DCI_USER_6_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_6_USERNAME_DEFAULT_ATTRIB ),
        DCI_USER_6_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 6 Password
        DCI_USER_6_PWD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_USER_6_PWD_RAM ),
        &DCI_USER_6_PWD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_USER_6_PWD_DEFAULT_ATTRIB ),
        DCI_USER_6_PWD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_USER_6_PWD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // User 6 Assigned Role
        DCI_USER_6_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_6_ROLE_RAM ),
        &DCI_USER_6_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_6_ROLE_DEFAULT_ATTRIB ),
        DCI_USER_6_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 6 Password Set Epoch Time
        DCI_USER_6_PWD_SET_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_6_PWD_SET_EPOCH_TIME_RAM ),
        &DCI_USER_6_PWD_SET_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_6_PWD_SET_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_6_PWD_SET_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 6 Last Login Epoch Time
        DCI_USER_6_LAST_LOGIN_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_6_LAST_LOGIN_EPOCH_TIME_RAM ),
        &DCI_USER_6_LAST_LOGIN_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_6_LAST_LOGIN_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_USER_6_LAST_LOGIN_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User 6 Password Complexity setting
        DCI_USER_6_PWD_COMPLEXITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_6_PWD_COMPLEXITY_RAM ),
        &DCI_USER_6_PWD_COMPLEXITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_6_PWD_COMPLEXITY_DEFAULT_ATTRIB ),
        DCI_USER_6_PWD_COMPLEXITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // User 6 Password Aging setting
        DCI_USER_6_PWD_AGING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_6_PWD_AGING_RAM ),
        &DCI_USER_6_PWD_AGING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_6_PWD_AGING_DEFAULT_ATTRIB ),
        DCI_USER_6_PWD_AGING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // HTTP enable or disable
        DCI_HTTP_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_HTTP_ENABLE_RAM ),
        &DCI_HTTP_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_HTTP_ENABLE_DEFAULT_ATTRIB ),
        DCI_HTTP_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_HTTP_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // IP_BIT_NOT_CONNECTED
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IP_BIT_NOT_CONNECTED_RAM ),
        &DCI_IP_BIT_NOT_CONNECTED_LENGTH,
        nullptr,
        DCI_IP_BIT_NOT_CONNECTED_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_IP_BIT_NOT_CONNECTED_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // IP_WORD_NOT_CONNECTED
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IP_WORD_NOT_CONNECTED_RAM ),
        &DCI_IP_WORD_NOT_CONNECTED_LENGTH,
        nullptr,
        DCI_IP_WORD_NOT_CONNECTED_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // IP_DWORD_NOT_CONNECTED
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IP_DWORD_NOT_CONNECTED_RAM ),
        &DCI_IP_DWORD_NOT_CONNECTED_LENGTH,
        nullptr,
        DCI_IP_DWORD_NOT_CONNECTED_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // OP_BIT_NOT_CONNECTED
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_OP_BIT_NOT_CONNECTED_RAM ),
        &DCI_OP_BIT_NOT_CONNECTED_LENGTH,
        nullptr,
        DCI_OP_BIT_NOT_CONNECTED_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_OP_BIT_NOT_CONNECTED_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // OP_WORD_NOT_CONNECTED
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_OP_WORD_NOT_CONNECTED_RAM ),
        &DCI_OP_WORD_NOT_CONNECTED_LENGTH,
        nullptr,
        DCI_OP_WORD_NOT_CONNECTED_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // OP_DWORD_NOT_CONNECTED
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_OP_DWORD_NOT_CONNECTED_RAM ),
        &DCI_OP_DWORD_NOT_CONNECTED_LENGTH,
        nullptr,
        DCI_OP_DWORD_NOT_CONNECTED_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Parameterization Download Enable
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DEVICE_PARAMETERIZATION_ENABLE_RAM ),
        &DCI_DEVICE_PARAMETERIZATION_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DEVICE_PARAMETERIZATION_ENABLE_DEFAULT_ATTRIB ),
        DCI_DEVICE_PARAMETERIZATION_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_DEVICE_PARAMETERIZATION_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Profibus Production Data
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_PROFIBUS_PROD_MOD_DATA_RAM ),
        &DCI_PROFIBUS_PROD_MOD_DATA_LENGTH,
        nullptr,
        DCI_PROFIBUS_PROD_MOD_DATA_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_RW_LENGTH_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Warning Status Bits
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_STATUS_WARNING_BITS_RAM ),
        &DCI_STATUS_WARNING_BITS_LENGTH,
        nullptr,
        DCI_STATUS_WARNING_BITS_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // Product Active Fault
        DCI_PRODUCT_ACTIVE_FAULT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PRODUCT_ACTIVE_FAULT_RAM ),
        &DCI_PRODUCT_ACTIVE_FAULT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PRODUCT_ACTIVE_FAULT_DEFAULT_ATTRIB ),
        DCI_PRODUCT_ACTIVE_FAULT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PRODUCT_ACTIVE_FAULT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Profibus Sample App Input Bit
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_APP_INPUT_BIT_RAM ),
        &DCI_SAMPLE_APP_INPUT_BIT_LENGTH,
        nullptr,
        DCI_SAMPLE_APP_INPUT_BIT_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // Profibus Sample App Input word
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_APP_INPUT_WORD_RAM ),
        &DCI_SAMPLE_APP_INPUT_WORD_LENGTH,
        nullptr,
        DCI_SAMPLE_APP_INPUT_WORD_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Profibus Sample App Input Dword
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_APP_INPUT_DWORD_RAM ),
        &DCI_SAMPLE_APP_INPUT_DWORD_LENGTH,
        nullptr,
        DCI_SAMPLE_APP_INPUT_DWORD_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Profibus Sample App Output Bit
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_APP_OUTPUT_BIT_RAM ),
        &DCI_SAMPLE_APP_OUTPUT_BIT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_APP_OUTPUT_BIT_DEFAULT_ATTRIB ),
        DCI_SAMPLE_APP_OUTPUT_BIT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Profibus Sample App Output word
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_APP_OUTPUT_WORD_RAM ),
        &DCI_SAMPLE_APP_OUTPUT_WORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_APP_OUTPUT_WORD_DEFAULT_ATTRIB ),
        DCI_SAMPLE_APP_OUTPUT_WORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_SAMPLE_APP_OUTPUT_WORD_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Profibus Sample App Output Dword
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_APP_OUTPUT_DWORD_RAM ),
        &DCI_SAMPLE_APP_OUTPUT_DWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_APP_OUTPUT_DWORD_DEFAULT_ATTRIB ),
        DCI_SAMPLE_APP_OUTPUT_DWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_SAMPLE_APP_OUTPUT_DWORD_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT32,
    },
    {        // Simple Input word
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_RAM ),
        &DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_DEFAULT_ATTRIB ),
        DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Profibus Sample App Parameter
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_APP_PARAMETER_RAM ),
        &DCI_SAMPLE_APP_PARAMETER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_APP_PARAMETER_DEFAULT_ATTRIB ),
        DCI_SAMPLE_APP_PARAMETER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_SAMPLE_APP_PARAMETER_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Profibus Sample Acyclic Parameter 1 
        DCI_SAMPLE_ACYCLIC_PARAMETER_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_ACYCLIC_PARAMETER_1_RAM ),
        &DCI_SAMPLE_ACYCLIC_PARAMETER_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_ACYCLIC_PARAMETER_1_DEFAULT_ATTRIB ),
        DCI_SAMPLE_ACYCLIC_PARAMETER_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Profibus Sample Acyclic Parameter 2 
        DCI_SAMPLE_ACYCLIC_PARAMETER_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_ACYCLIC_PARAMETER_2_RAM ),
        &DCI_SAMPLE_ACYCLIC_PARAMETER_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_ACYCLIC_PARAMETER_2_DEFAULT_ATTRIB ),
        DCI_SAMPLE_ACYCLIC_PARAMETER_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Profibus Sample Acyclic  Parameter 3 
        DCI_SAMPLE_ACYCLIC_PARAMETER_3_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_ACYCLIC_PARAMETER_3_RAM ),
        &DCI_SAMPLE_ACYCLIC_PARAMETER_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_ACYCLIC_PARAMETER_3_DEFAULT_ATTRIB ),
        DCI_SAMPLE_ACYCLIC_PARAMETER_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Profibus Sample Acyclic Parameter 4 
        DCI_SAMPLE_ACYCLIC_PARAMETER_4_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_ACYCLIC_PARAMETER_4_RAM ),
        &DCI_SAMPLE_ACYCLIC_PARAMETER_4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_ACYCLIC_PARAMETER_4_DEFAULT_ATTRIB ),
        DCI_SAMPLE_ACYCLIC_PARAMETER_4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Profibus Sample Acyclic  Parameter 5 
        DCI_SAMPLE_ACYCLIC_PARAMETER_5_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_ACYCLIC_PARAMETER_5_RAM ),
        &DCI_SAMPLE_ACYCLIC_PARAMETER_5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_ACYCLIC_PARAMETER_5_DEFAULT_ATTRIB ),
        DCI_SAMPLE_ACYCLIC_PARAMETER_5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Profibus Sample Acyclic  Parameter 6 
        DCI_SAMPLE_ACYCLIC_PARAMETER_6_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SAMPLE_ACYCLIC_PARAMETER_6_RAM ),
        &DCI_SAMPLE_ACYCLIC_PARAMETER_6_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SAMPLE_ACYCLIC_PARAMETER_6_DEFAULT_ATTRIB ),
        DCI_SAMPLE_ACYCLIC_PARAMETER_6_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Input Test Bit 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_INPUT_TEST_BIT_1_RAM ),
        &DCI_INPUT_TEST_BIT_1_LENGTH,
        nullptr,
        DCI_INPUT_TEST_BIT_1_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Output Test Bit 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_OUTPUT_TEST_BIT_1_RAM ),
        &DCI_OUTPUT_TEST_BIT_1_LENGTH,
        nullptr,
        DCI_OUTPUT_TEST_BIT_1_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Eaton Vendor ID
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CIP_VENDOR_ID_DEFAULT_ATTRIB )),
        &DCI_CIP_VENDOR_ID_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CIP_VENDOR_ID_DEFAULT_ATTRIB ),
        DCI_CIP_VENDOR_ID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // CIP Device Type
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CIP_DEVICE_TYPE_DEFAULT_ATTRIB )),
        &DCI_CIP_DEVICE_TYPE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CIP_DEVICE_TYPE_DEFAULT_ATTRIB ),
        DCI_CIP_DEVICE_TYPE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // CIP Device Identity Device Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CIP_DEVICE_IDENT_DEV_STATUS_RAM ),
        &DCI_CIP_DEVICE_IDENT_DEV_STATUS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CIP_DEVICE_IDENT_DEV_STATUS_DEFAULT_ATTRIB ),
        DCI_CIP_DEVICE_IDENT_DEV_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Name Server 1 For CIP Ethernet
        DCI_ETH_NAME_SERVER_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ETH_NAME_SERVER_1_RAM ),
        &DCI_ETH_NAME_SERVER_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ETH_NAME_SERVER_1_DEFAULT_ATTRIB ),
        DCI_ETH_NAME_SERVER_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Name Server 2 For CIP Ethernet
        DCI_ETH_NAME_SERVER_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ETH_NAME_SERVER_2_RAM ),
        &DCI_ETH_NAME_SERVER_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ETH_NAME_SERVER_2_DEFAULT_ATTRIB ),
        DCI_ETH_NAME_SERVER_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Domain Name For CIP Ethernet
        DCI_ETH_DOMAIN_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_ETH_DOMAIN_NAME_RAM ),
        &DCI_ETH_DOMAIN_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_ETH_DOMAIN_NAME_DEFAULT_ATTRIB ),
        DCI_ETH_DOMAIN_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Host Name for Device
        DCI_ETH_HOST_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_ETH_HOST_NAME_RAM ),
        &DCI_ETH_HOST_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_ETH_HOST_NAME_DEFAULT_ATTRIB ),
        DCI_ETH_HOST_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Encapsulation Inactivity Timeout
        DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_RAM ),
        &DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_DEFAULT_ATTRIB ),
        DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Application Firmware Version
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_APP_FIRM_VER_RAM ),
        &DCI_APP_FIRM_VER_LENGTH,
        nullptr,
        DCI_APP_FIRM_VER_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // EIP input assembly test
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EIP_TEST_INPUT_ASM_RAM ),
        &DCI_EIP_TEST_INPUT_ASM_LENGTH,
        nullptr,
        DCI_EIP_TEST_INPUT_ASM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RW_LENGTH_MASK )
        ),
        ( 
            0U
        ),
        DCI_EIP_TEST_INPUT_ASM_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // EIP output assembly test
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EIP_TEST_OUTPUT_ASM_RAM ),
        &DCI_EIP_TEST_OUTPUT_ASM_LENGTH,
        nullptr,
        DCI_EIP_TEST_OUTPUT_ASM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RW_LENGTH_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_EIP_TEST_OUTPUT_ASM_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // CIP Device Identity State
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CIP_DEVICE_IDENT_STATE_RAM ),
        &DCI_CIP_DEVICE_IDENT_STATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CIP_DEVICE_IDENT_STATE_DEFAULT_ATTRIB ),
        DCI_CIP_DEVICE_IDENT_STATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Dynamic i/p Assembly terminator
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_TERMINATOR_DEFAULT_ATTRIB )),
        &DCI_DYN_IN_ASM_TERMINATOR_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_TERMINATOR_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_TERMINATOR_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p Assembly (Instance 150) member 1
        DCI_DYN_IN_ASM_MEMBER_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_MEMBER_1_RAM ),
        &DCI_DYN_IN_ASM_MEMBER_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_MEMBER_1_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_MEMBER_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p Assembly (Instance 150) member 2
        DCI_DYN_IN_ASM_MEMBER_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_MEMBER_2_RAM ),
        &DCI_DYN_IN_ASM_MEMBER_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_MEMBER_2_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_MEMBER_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p Assembly (Instance 150) member 3
        DCI_DYN_IN_ASM_MEMBER_3_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_MEMBER_3_RAM ),
        &DCI_DYN_IN_ASM_MEMBER_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_MEMBER_3_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_MEMBER_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p Assembly (Instance 150) member 4
        DCI_DYN_IN_ASM_MEMBER_4_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_MEMBER_4_RAM ),
        &DCI_DYN_IN_ASM_MEMBER_4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_MEMBER_4_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_MEMBER_4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p Assembly (Instance 150) member 5
        DCI_DYN_IN_ASM_MEMBER_5_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_MEMBER_5_RAM ),
        &DCI_DYN_IN_ASM_MEMBER_5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_MEMBER_5_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_MEMBER_5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p Assembly (Instance 150) member 6
        DCI_DYN_IN_ASM_MEMBER_6_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_MEMBER_6_RAM ),
        &DCI_DYN_IN_ASM_MEMBER_6_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_MEMBER_6_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_MEMBER_6_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p Assembly (Instance 150) member 7
        DCI_DYN_IN_ASM_MEMBER_7_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_MEMBER_7_RAM ),
        &DCI_DYN_IN_ASM_MEMBER_7_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_MEMBER_7_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_MEMBER_7_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p Assembly (Instance 150) member 8
        DCI_DYN_IN_ASM_MEMBER_8_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_MEMBER_8_RAM ),
        &DCI_DYN_IN_ASM_MEMBER_8_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_MEMBER_8_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_MEMBER_8_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic i/p assembly (Instance 150)
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_INTERFACE_RAM ),
        &DCI_DYN_IN_ASM_INTERFACE_LENGTH,
        nullptr,
        DCI_DYN_IN_ASM_INTERFACE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RW_LENGTH_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_INTERFACE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Dynamic i/p Assembly (Instance 150) Selection1
        DCI_DYN_IN_ASM_SELECT_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_SELECT_1_RAM ),
        &DCI_DYN_IN_ASM_SELECT_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_1_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_SELECT_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_SELECT_1_CBACK_INDEX,        //Not Default
        DCI_DYN_IN_ASM_SELECT_1_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic i/p Assembly (Instance 150) Selection2
        DCI_DYN_IN_ASM_SELECT_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_SELECT_2_RAM ),
        &DCI_DYN_IN_ASM_SELECT_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_2_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_SELECT_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_SELECT_2_CBACK_INDEX,        //Not Default
        DCI_DYN_IN_ASM_SELECT_2_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic i/p Assembly (Instance 150) Selection3
        DCI_DYN_IN_ASM_SELECT_3_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_SELECT_3_RAM ),
        &DCI_DYN_IN_ASM_SELECT_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_3_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_SELECT_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_SELECT_3_CBACK_INDEX,        //Not Default
        DCI_DYN_IN_ASM_SELECT_3_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic i/p Assembly (Instance 150) Selection4
        DCI_DYN_IN_ASM_SELECT_4_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_SELECT_4_RAM ),
        &DCI_DYN_IN_ASM_SELECT_4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_4_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_SELECT_4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_SELECT_4_CBACK_INDEX,        //Not Default
        DCI_DYN_IN_ASM_SELECT_4_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic i/p Assembly (Instance 150) Selection5
        DCI_DYN_IN_ASM_SELECT_5_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_SELECT_5_RAM ),
        &DCI_DYN_IN_ASM_SELECT_5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_5_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_SELECT_5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_SELECT_5_CBACK_INDEX,        //Not Default
        DCI_DYN_IN_ASM_SELECT_5_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic i/p Assembly (Instance 150) Selection6
        DCI_DYN_IN_ASM_SELECT_6_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_SELECT_6_RAM ),
        &DCI_DYN_IN_ASM_SELECT_6_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_6_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_SELECT_6_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_SELECT_6_CBACK_INDEX,        //Not Default
        DCI_DYN_IN_ASM_SELECT_6_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic i/p Assembly (Instance 150) Selection7
        DCI_DYN_IN_ASM_SELECT_7_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_SELECT_7_RAM ),
        &DCI_DYN_IN_ASM_SELECT_7_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_7_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_SELECT_7_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_SELECT_7_CBACK_INDEX,        //Not Default
        DCI_DYN_IN_ASM_SELECT_7_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic i/p Assembly (Instance 150) Selection8
        DCI_DYN_IN_ASM_SELECT_8_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_IN_ASM_SELECT_8_RAM ),
        &DCI_DYN_IN_ASM_SELECT_8_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_IN_ASM_SELECT_8_DEFAULT_ATTRIB ),
        DCI_DYN_IN_ASM_SELECT_8_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_IN_ASM_SELECT_8_CBACK_INDEX,        //Not Default
        DCI_DYN_IN_ASM_SELECT_8_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic o/p Assembly terminator
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_TERMINATOR_DEFAULT_ATTRIB )),
        &DCI_DYN_OUT_ASM_TERMINATOR_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_TERMINATOR_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_TERMINATOR_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic o/p Assembly (Instance 150) member 1
        DCI_DYN_OUT_ASM_MEMBER_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_MEMBER_1_RAM ),
        &DCI_DYN_OUT_ASM_MEMBER_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_MEMBER_1_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_MEMBER_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic o/p Assembly (Instance 150) member 2
        DCI_DYN_OUT_ASM_MEMBER_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_MEMBER_2_RAM ),
        &DCI_DYN_OUT_ASM_MEMBER_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_MEMBER_2_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_MEMBER_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic o/p Assembly (Instance 150) member 3
        DCI_DYN_OUT_ASM_MEMBER_3_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_MEMBER_3_RAM ),
        &DCI_DYN_OUT_ASM_MEMBER_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_MEMBER_3_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_MEMBER_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic o/p Assembly (Instance 150) member 4
        DCI_DYN_OUT_ASM_MEMBER_4_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_MEMBER_4_RAM ),
        &DCI_DYN_OUT_ASM_MEMBER_4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_MEMBER_4_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_MEMBER_4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Dynamic o/p assembly
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_INTERFACE_RAM ),
        &DCI_DYN_OUT_ASM_INTERFACE_LENGTH,
        nullptr,
        DCI_DYN_OUT_ASM_INTERFACE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RW_LENGTH_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_DYN_OUT_ASM_INTERFACE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Dynamic o/p Assembly (Instance 150) Selection1
        DCI_DYN_OUT_ASM_SELECT_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_SELECT_1_RAM ),
        &DCI_DYN_OUT_ASM_SELECT_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_1_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_SELECT_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_OUT_ASM_SELECT_1_CBACK_INDEX,        //Not Default
        DCI_DYN_OUT_ASM_SELECT_1_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic o/p Assembly (Instance 150) Selection2
        DCI_DYN_OUT_ASM_SELECT_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_SELECT_2_RAM ),
        &DCI_DYN_OUT_ASM_SELECT_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_2_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_SELECT_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_OUT_ASM_SELECT_2_CBACK_INDEX,        //Not Default
        DCI_DYN_OUT_ASM_SELECT_2_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic o/p Assembly (Instance 150) Selection3
        DCI_DYN_OUT_ASM_SELECT_3_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_SELECT_3_RAM ),
        &DCI_DYN_OUT_ASM_SELECT_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_3_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_SELECT_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_OUT_ASM_SELECT_3_CBACK_INDEX,        //Not Default
        DCI_DYN_OUT_ASM_SELECT_3_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Dynamic o/p Assembly (Instance 150) Selection4
        DCI_DYN_OUT_ASM_SELECT_4_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DYN_OUT_ASM_SELECT_4_RAM ),
        &DCI_DYN_OUT_ASM_SELECT_4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DYN_OUT_ASM_SELECT_4_DEFAULT_ATTRIB ),
        DCI_DYN_OUT_ASM_SELECT_4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_DYN_OUT_ASM_SELECT_4_CBACK_INDEX,        //Not Default
        DCI_DYN_OUT_ASM_SELECT_4_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // DHCP to Static IP addr lock
        DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_RAM ),
        &DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_DEFAULT_ATTRIB ),
        DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Fault Queue - Sorted List
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_LTK_FAULT_QUEUE_SORTED_RAM ),
        &DCI_LTK_FAULT_QUEUE_SORTED_LENGTH,
        nullptr,
        DCI_LTK_FAULT_QUEUE_SORTED_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Fault Queue - Event List
        DCI_LTK_FAULT_QUEUE_EVENT_ORDER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_LTK_FAULT_QUEUE_EVENT_ORDER_RAM ),
        &DCI_LTK_FAULT_QUEUE_EVENT_ORDER_LENGTH,
        nullptr,
        DCI_LTK_FAULT_QUEUE_EVENT_ORDER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Fault Status Bitfield
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LTK_FAULT_STATUS_BITS_RAM ),
        &DCI_LTK_FAULT_STATUS_BITS_LENGTH,
        nullptr,
        DCI_LTK_FAULT_STATUS_BITS_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // Fault Reset
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_FAULT_RESET_RAM ),
        &DCI_FAULT_RESET_LENGTH,
        nullptr,
        DCI_FAULT_RESET_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_FAULT_RESET_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // EIP Test Reg 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EIP_TEST1_RAM ),
        &DCI_EIP_TEST1_LENGTH,
        nullptr,
        DCI_EIP_TEST1_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // EIP Test Reg 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EIP_TEST2_RAM ),
        &DCI_EIP_TEST2_LENGTH,
        nullptr,
        DCI_EIP_TEST2_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // EIP Test Reg 3
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EIP_TEST3_RAM ),
        &DCI_EIP_TEST3_LENGTH,
        nullptr,
        DCI_EIP_TEST3_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // EIP Test Reg 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EIP_TEST4_RAM ),
        &DCI_EIP_TEST4_LENGTH,
        nullptr,
        DCI_EIP_TEST4_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // BACnet Vendor Identifier
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_VENDOR_IDENTIFIER_DEFAULT_ATTRIB )),
        &DCI_BACNET_VENDOR_IDENTIFIER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_VENDOR_IDENTIFIER_DEFAULT_ATTRIB ),
        DCI_BACNET_VENDOR_IDENTIFIER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // BACnet Max Master
        DCI_MAX_MASTER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MAX_MASTER_RAM ),
        &DCI_MAX_MASTER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MAX_MASTER_DEFAULT_ATTRIB ),
        DCI_MAX_MASTER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // BACnet Max Info Frames
        DCI_MAX_INFO_FRAMES_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MAX_INFO_FRAMES_RAM ),
        &DCI_MAX_INFO_FRAMES_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MAX_INFO_FRAMES_DEFAULT_ATTRIB ),
        DCI_MAX_INFO_FRAMES_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // BACnet Profile Name
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_PROFILE_NAME_DEFAULT_ATTRIB )),
        &DCI_PROFILE_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_PROFILE_NAME_DEFAULT_ATTRIB ),
        DCI_PROFILE_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // BACnet IP UDP port number
        DCI_BACNET_IP_UPD_PORT_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_UPD_PORT_NUM_RAM ),
        &DCI_BACNET_IP_UPD_PORT_NUM_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_IP_UPD_PORT_NUM_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_UPD_PORT_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_IP_UPD_PORT_NUM_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // BACnet IP Foreign Device
        DCI_BACNET_IP_FOREIGN_DEVICE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_FOREIGN_DEVICE_RAM ),
        &DCI_BACNET_IP_FOREIGN_DEVICE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_IP_FOREIGN_DEVICE_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_FOREIGN_DEVICE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_IP_FOREIGN_DEVICE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // BACnet IP BBMD IP
        DCI_BACNET_IP_BBMD_IP_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BACNET_IP_BBMD_IP_RAM ),
        &DCI_BACNET_IP_BBMD_IP_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BACNET_IP_BBMD_IP_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_BBMD_IP_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // BACnet IP BBMD Port
        DCI_BACNET_IP_BBMD_PORT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_BBMD_PORT_RAM ),
        &DCI_BACNET_IP_BBMD_PORT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_IP_BBMD_PORT_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_BBMD_PORT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_IP_BBMD_PORT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // BACnet IP Registration Interval
        DCI_BACNET_IP_REGISTRATION_INTERVAL_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_REGISTRATION_INTERVAL_RAM ),
        &DCI_BACNET_IP_REGISTRATION_INTERVAL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_IP_REGISTRATION_INTERVAL_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_REGISTRATION_INTERVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // BACnet IP Comm Timeout
        DCI_BACNET_IP_COMM_TIMEOUT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_COMM_TIMEOUT_RAM ),
        &DCI_BACNET_IP_COMM_TIMEOUT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_IP_COMM_TIMEOUT_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_COMM_TIMEOUT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_IP_COMM_TIMEOUT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // BACnet IP Protocol Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_PROTOCOL_STATUS_RAM ),
        &DCI_BACNET_IP_PROTOCOL_STATUS_LENGTH,
        nullptr,
        DCI_BACNET_IP_PROTOCOL_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_IP_PROTOCOL_STATUS_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // BACnet IP Fault Response
        DCI_BACNET_IP_FAULT_BEHAVIOR_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_FAULT_BEHAVIOR_RAM ),
        &DCI_BACNET_IP_FAULT_BEHAVIOR_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_IP_FAULT_BEHAVIOR_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_FAULT_BEHAVIOR_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_IP_FAULT_BEHAVIOR_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Enable BACnet IP
        DCI_ENABLE_BACNET_IP_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ENABLE_BACNET_IP_RAM ),
        &DCI_ENABLE_BACNET_IP_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ENABLE_BACNET_IP_DEFAULT_ATTRIB ),
        DCI_ENABLE_BACNET_IP_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_ENABLE_BACNET_IP_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // BACnet MS/TP Device Address
        DCI_BACNET_DEVICE_ADDR_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_DEVICE_ADDR_RAM ),
        &DCI_BACNET_DEVICE_ADDR_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_DEVICE_ADDR_DEFAULT_ATTRIB ),
        DCI_BACNET_DEVICE_ADDR_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_DEVICE_ADDR_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // BACnet IP Instance Number
        DCI_BACNET_IP_INSTANCE_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_INSTANCE_NUM_RAM ),
        &DCI_BACNET_IP_INSTANCE_NUM_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_IP_INSTANCE_NUM_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_INSTANCE_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_IP_INSTANCE_NUM_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT32,
    },
    {        // BACnet MS/TP Instance Number
        DCI_BACNET_MSTP_INSTANCE_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_MSTP_INSTANCE_NUM_RAM ),
        &DCI_BACNET_MSTP_INSTANCE_NUM_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_MSTP_INSTANCE_NUM_DEFAULT_ATTRIB ),
        DCI_BACNET_MSTP_INSTANCE_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_MSTP_INSTANCE_NUM_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT32,
    },
    {        // BACnet MS/TP Communication Timeout
        DCI_BACNET_MSTP_COMM_TIMEOUT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_MSTP_COMM_TIMEOUT_RAM ),
        &DCI_BACNET_MSTP_COMM_TIMEOUT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_MSTP_COMM_TIMEOUT_DEFAULT_ATTRIB ),
        DCI_BACNET_MSTP_COMM_TIMEOUT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_MSTP_COMM_TIMEOUT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // BACnet MSTP UART baudrate
        DCI_BACNET_MSTP_BAUD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_MSTP_BAUD_RAM ),
        &DCI_BACNET_MSTP_BAUD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_MSTP_BAUD_DEFAULT_ATTRIB ),
        DCI_BACNET_MSTP_BAUD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_MSTP_BAUD_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT32,
    },
    {        // BACnet MSTP UART parity
        DCI_BACNET_MSTP_PARITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_MSTP_PARITY_RAM ),
        &DCI_BACNET_MSTP_PARITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_MSTP_PARITY_DEFAULT_ATTRIB ),
        DCI_BACNET_MSTP_PARITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_MSTP_PARITY_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // BACnet MSTP UART stopbit
        DCI_BACNET_MSTP_STOPBIT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_MSTP_STOPBIT_RAM ),
        &DCI_BACNET_MSTP_STOPBIT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_MSTP_STOPBIT_DEFAULT_ATTRIB ),
        DCI_BACNET_MSTP_STOPBIT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_MSTP_STOPBIT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // BACnet MS/TP FieldBus Protocol Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_MSTP_PROTOCOL_STATUS_RAM ),
        &DCI_BACNET_MSTP_PROTOCOL_STATUS_LENGTH,
        nullptr,
        DCI_BACNET_MSTP_PROTOCOL_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_MSTP_PROTOCOL_STATUS_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // BACnet MS/TP Fault code
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_FAULT_CODE_RAM ),
        &DCI_BACNET_FAULT_CODE_LENGTH,
        nullptr,
        DCI_BACNET_FAULT_CODE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_FAULT_CODE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // BACnet Database Revision
        DCI_BACNET_DATABASE_REVISION_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_DATABASE_REVISION_RAM ),
        &DCI_BACNET_DATABASE_REVISION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_DATABASE_REVISION_DEFAULT_ATTRIB ),
        DCI_BACNET_DATABASE_REVISION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Analog Value 0
        DCI_ANALOG_VALUE_0_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ANALOG_VALUE_0_RAM ),
        &DCI_ANALOG_VALUE_0_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ANALOG_VALUE_0_DEFAULT_ATTRIB ),
        DCI_ANALOG_VALUE_0_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Analog Value 1
        DCI_ANALOG_VALUE_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ANALOG_VALUE_1_RAM ),
        &DCI_ANALOG_VALUE_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ANALOG_VALUE_1_DEFAULT_ATTRIB ),
        DCI_ANALOG_VALUE_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // AnyParam ID
        DCI_AV_ANY_PARAM_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_AV_ANY_PARAM_NUM_RAM ),
        &DCI_AV_ANY_PARAM_NUM_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AV_ANY_PARAM_NUM_DEFAULT_ATTRIB ),
        DCI_AV_ANY_PARAM_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_AV_ANY_PARAM_NUM_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // AnyParam Value
        DCI_AV_ANY_PARAM_VAL_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_AV_ANY_PARAM_VAL_RAM ),
        &DCI_AV_ANY_PARAM_VAL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AV_ANY_PARAM_VAL_DEFAULT_ATTRIB ),
        DCI_AV_ANY_PARAM_VAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_AV_ANY_PARAM_VAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Analog Input 0
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ANALOG_INPUT_0_RAM ),
        &DCI_ANALOG_INPUT_0_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ANALOG_INPUT_0_DEFAULT_ATTRIB ),
        DCI_ANALOG_INPUT_0_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Analog Input 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ANALOG_INPUT_1_RAM ),
        &DCI_ANALOG_INPUT_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ANALOG_INPUT_1_DEFAULT_ATTRIB ),
        DCI_ANALOG_INPUT_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Digital Input word
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DIGITAL_INPUT_WORD_RAM ),
        &DCI_DIGITAL_INPUT_WORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DIGITAL_INPUT_WORD_DEFAULT_ATTRIB ),
        DCI_DIGITAL_INPUT_WORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Digital Output word
        DCI_DIGITAL_OUTPUT_WORD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DIGITAL_OUTPUT_WORD_RAM ),
        &DCI_DIGITAL_OUTPUT_WORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DIGITAL_OUTPUT_WORD_DEFAULT_ATTRIB ),
        DCI_DIGITAL_OUTPUT_WORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Digital Input bit
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DIGITAL_INPUT_BIT_RAM ),
        &DCI_DIGITAL_INPUT_BIT_LENGTH,
        nullptr,
        DCI_DIGITAL_INPUT_BIT_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Digital Output bit
        DCI_DIGITAL_OUTPUT_BIT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DIGITAL_OUTPUT_BIT_RAM ),
        &DCI_DIGITAL_OUTPUT_BIT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DIGITAL_OUTPUT_BIT_DEFAULT_ATTRIB ),
        DCI_DIGITAL_OUTPUT_BIT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Multi state 0
        DCI_MULTI_STATE_0_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MULTI_STATE_0_RAM ),
        &DCI_MULTI_STATE_0_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MULTI_STATE_0_DEFAULT_ATTRIB ),
        DCI_MULTI_STATE_0_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Multi state 1
        DCI_MULTI_STATE_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MULTI_STATE_1_RAM ),
        &DCI_MULTI_STATE_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MULTI_STATE_1_DEFAULT_ATTRIB ),
        DCI_MULTI_STATE_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Trusted IP Adress filter white list
        DCI_TRUSTED_IP_WHITELIST_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_TRUSTED_IP_WHITELIST_RAM ),
        &DCI_TRUSTED_IP_WHITELIST_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_TRUSTED_IP_WHITELIST_DEFAULT_ATTRIB ),
        DCI_TRUSTED_IP_WHITELIST_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Trusted IP filter for Modbus/TCP Enable
        DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_RAM ),
        &DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_DEFAULT_ATTRIB ),
        DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Trusted IP filter for BACnet IP Enable
        DCI_TRUSTED_IP_BACNET_IP_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_TRUSTED_IP_BACNET_IP_ENABLE_RAM ),
        &DCI_TRUSTED_IP_BACNET_IP_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_TRUSTED_IP_BACNET_IP_ENABLE_DEFAULT_ATTRIB ),
        DCI_TRUSTED_IP_BACNET_IP_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Trusted IP filter for EIP Enable
        DCI_TRUSTED_IP_EIP_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_TRUSTED_IP_EIP_ENABLE_RAM ),
        &DCI_TRUSTED_IP_EIP_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_TRUSTED_IP_EIP_ENABLE_DEFAULT_ATTRIB ),
        DCI_TRUSTED_IP_EIP_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Unix Epoch Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_UNIX_EPOCH_TIME_RAM ),
        &DCI_UNIX_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_UNIX_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_UNIX_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_UNIX_EPOCH_TIME_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Unix Epoch Time_64bit
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_UNIX_EPOCH_TIME_64_BIT_RAM ),
        &DCI_UNIX_EPOCH_TIME_64_BIT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_UNIX_EPOCH_TIME_64_BIT_DEFAULT_ATTRIB ),
        DCI_UNIX_EPOCH_TIME_64_BIT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_UNIX_EPOCH_TIME_64_BIT_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Time Offset
        DCI_TIME_OFFSET_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_TIME_OFFSET_RAM ),
        &DCI_TIME_OFFSET_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_TIME_OFFSET_DEFAULT_ATTRIB ),
        DCI_TIME_OFFSET_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_TIME_OFFSET_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT16,
    },
    {        // Date Format
        DCI_DATE_FORMAT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DATE_FORMAT_RAM ),
        &DCI_DATE_FORMAT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DATE_FORMAT_DEFAULT_ATTRIB ),
        DCI_DATE_FORMAT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_DATE_FORMAT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Time Format
        DCI_TIME_FORMAT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_TIME_FORMAT_RAM ),
        &DCI_TIME_FORMAT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_TIME_FORMAT_DEFAULT_ATTRIB ),
        DCI_TIME_FORMAT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_TIME_FORMAT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // REST Reset Command
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_REST_RESET_COMMAND_RAM ),
        &DCI_REST_RESET_COMMAND_LENGTH,
        nullptr,
        DCI_REST_RESET_COMMAND_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        DCI_REST_RESET_COMMAND_CBACK_INDEX,        //Not Default
        DCI_REST_RESET_COMMAND_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Device Certificate Validity Status
        DCI_DEVICE_CERT_VALID_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DEVICE_CERT_VALID_RAM ),
        &DCI_DEVICE_CERT_VALID_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DEVICE_CERT_VALID_DEFAULT_ATTRIB ),
        DCI_DEVICE_CERT_VALID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_DEVICE_CERT_VALID_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Device Certificate Generation Control
        DCI_DEVICE_CERT_CONTROL_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DEVICE_CERT_CONTROL_RAM ),
        &DCI_DEVICE_CERT_CONTROL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DEVICE_CERT_CONTROL_DEFAULT_ATTRIB ),
        DCI_DEVICE_CERT_CONTROL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        DCI_DEVICE_CERT_CONTROL_CBACK_INDEX,        //Not Default
        DCI_DEVICE_CERT_CONTROL_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Internal fault catcher for really bad faults.
        DCI_FAULT_CATCHER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_FAULT_CATCHER_RAM ),
        &DCI_FAULT_CATCHER_LENGTH,
        nullptr,
        DCI_FAULT_CATCHER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Sntp operation Enable
        DCI_SNTP_SERVICE_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SNTP_SERVICE_ENABLE_RAM ),
        &DCI_SNTP_SERVICE_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SNTP_SERVICE_ENABLE_DEFAULT_ATTRIB ),
        DCI_SNTP_SERVICE_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        DCI_SNTP_SERVICE_ENABLE_CBACK_INDEX,        //Not Default
        DCI_SNTP_SERVICE_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Time Server 1
        DCI_SNTP_SERVER_1_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_SNTP_SERVER_1_RAM ),
        &DCI_SNTP_SERVER_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_SNTP_SERVER_1_DEFAULT_ATTRIB ),
        DCI_SNTP_SERVER_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_SNTP_SERVER_1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Time Server 2
        DCI_SNTP_SERVER_2_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_SNTP_SERVER_2_RAM ),
        &DCI_SNTP_SERVER_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_SNTP_SERVER_2_DEFAULT_ATTRIB ),
        DCI_SNTP_SERVER_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_SNTP_SERVER_2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Time Server 3
        DCI_SNTP_SERVER_3_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_SNTP_SERVER_3_RAM ),
        &DCI_SNTP_SERVER_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_SNTP_SERVER_3_DEFAULT_ATTRIB ),
        DCI_SNTP_SERVER_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_SNTP_SERVER_3_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Sntp Update Time
        DCI_SNTP_UPDATE_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SNTP_UPDATE_TIME_RAM ),
        &DCI_SNTP_UPDATE_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SNTP_UPDATE_TIME_DEFAULT_ATTRIB ),
        DCI_SNTP_UPDATE_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_SNTP_UPDATE_TIME_CBACK_INDEX,        //Not Default
        DCI_SNTP_UPDATE_TIME_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT32,
    },
    {        // Sntp Retry Time
        DCI_SNTP_RETRY_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SNTP_RETRY_TIME_RAM ),
        &DCI_SNTP_RETRY_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SNTP_RETRY_TIME_DEFAULT_ATTRIB ),
        DCI_SNTP_RETRY_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        DCI_SNTP_RETRY_TIME_CBACK_INDEX,        //Not Default
        DCI_SNTP_RETRY_TIME_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Sntp Active Server Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SNTP_ACTIVE_SERVER_RAM ),
        &DCI_SNTP_ACTIVE_SERVER_LENGTH,
        nullptr,
        DCI_SNTP_ACTIVE_SERVER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_SNTP_ACTIVE_SERVER_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // IOT Enable or Disable
        DCI_IOT_CONNECT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_CONNECT_RAM ),
        &DCI_IOT_CONNECT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_CONNECT_DEFAULT_ATTRIB ),
        DCI_IOT_CONNECT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_CONNECT_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // IOT Connection Status
        DCI_IOT_STATUS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_STATUS_RAM ),
        &DCI_IOT_STATUS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_STATUS_DEFAULT_ATTRIB ),
        DCI_IOT_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Proxy Enable
        DCI_IOT_PROXY_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_PROXY_ENABLE_RAM ),
        &DCI_IOT_PROXY_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_PROXY_ENABLE_DEFAULT_ATTRIB ),
        DCI_IOT_PROXY_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PROXY_ENABLE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Proxy Server Address
        DCI_IOT_PROXY_SERVER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_PROXY_SERVER_RAM ),
        &DCI_IOT_PROXY_SERVER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_PROXY_SERVER_DEFAULT_ATTRIB ),
        DCI_IOT_PROXY_SERVER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PROXY_SERVER_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Proxy Server Port
        DCI_IOT_PROXY_PORT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_PROXY_PORT_RAM ),
        &DCI_IOT_PROXY_PORT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_PROXY_PORT_DEFAULT_ATTRIB ),
        DCI_IOT_PROXY_PORT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PROXY_PORT_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Proxy Username
        DCI_IOT_PROXY_USERNAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_PROXY_USERNAME_RAM ),
        &DCI_IOT_PROXY_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_PROXY_USERNAME_DEFAULT_ATTRIB ),
        DCI_IOT_PROXY_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PROXY_USERNAME_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Proxy Password
        DCI_IOT_PROXY_PASSWORD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_PROXY_PASSWORD_RAM ),
        &DCI_IOT_PROXY_PASSWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_PROXY_PASSWORD_DEFAULT_ATTRIB ),
        DCI_IOT_PROXY_PASSWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PROXY_PASSWORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Device GUID
        DCI_IOT_DEVICE_GUID_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_DEVICE_GUID_RAM ),
        &DCI_IOT_DEVICE_GUID_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_DEVICE_GUID_DEFAULT_ATTRIB ),
        DCI_IOT_DEVICE_GUID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_DEVICE_GUID_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Device Profile
        DCI_IOT_DEVICE_PROFILE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_DEVICE_PROFILE_RAM ),
        &DCI_IOT_DEVICE_PROFILE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_DEVICE_PROFILE_DEFAULT_ATTRIB ),
        DCI_IOT_DEVICE_PROFILE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_DEVICE_PROFILE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // IOT Hub Server Connection string
        DCI_IOT_CONN_STRING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_CONN_STRING_RAM ),
        &DCI_IOT_CONN_STRING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_CONN_STRING_DEFAULT_ATTRIB ),
        DCI_IOT_CONN_STRING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_CONN_STRING_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // IOT Data Update Rate
        DCI_IOT_UPDATE_RATE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_UPDATE_RATE_RAM ),
        &DCI_IOT_UPDATE_RATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_UPDATE_RATE_DEFAULT_ATTRIB ),
        DCI_IOT_UPDATE_RATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_IOT_UPDATE_RATE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // IOT Cadence Update Rate
        DCI_IOT_CADENCE_UPDATE_RATE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_CADENCE_UPDATE_RATE_RAM ),
        &DCI_IOT_CADENCE_UPDATE_RATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_CADENCE_UPDATE_RATE_DEFAULT_ATTRIB ),
        DCI_IOT_CADENCE_UPDATE_RATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_IOT_CADENCE_UPDATE_RATE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Group 0 Cadence - Normal SeeMe
        DCI_GROUP0_CADENCE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_GROUP0_CADENCE_RAM ),
        &DCI_GROUP0_CADENCE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_GROUP0_CADENCE_DEFAULT_ATTRIB ),
        DCI_GROUP0_CADENCE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        DCI_GROUP0_CADENCE_CBACK_INDEX,        //Not Default
        DCI_GROUP0_CADENCE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT32,
    },
    {        // Group 1 Cadence - 1 minute StoreMe
        DCI_GROUP1_CADENCE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_GROUP1_CADENCE_RAM ),
        &DCI_GROUP1_CADENCE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_GROUP1_CADENCE_DEFAULT_ATTRIB ),
        DCI_GROUP1_CADENCE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        DCI_GROUP1_CADENCE_CBACK_INDEX,        //Not Default
        DCI_GROUP1_CADENCE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT32,
    },
    {        // Group 2 Cadence - Fast SeeMe
        DCI_GROUP2_CADENCE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_GROUP2_CADENCE_RAM ),
        &DCI_GROUP2_CADENCE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_GROUP2_CADENCE_DEFAULT_ATTRIB ),
        DCI_GROUP2_CADENCE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        DCI_GROUP2_CADENCE_CBACK_INDEX,        //Not Default
        DCI_GROUP2_CADENCE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT32,
    },
    {        // Group 3 Cadence - StoreMe on change
        DCI_GROUP3_CADENCE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_GROUP3_CADENCE_RAM ),
        &DCI_GROUP3_CADENCE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_GROUP3_CADENCE_DEFAULT_ATTRIB ),
        DCI_GROUP3_CADENCE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        DCI_GROUP3_CADENCE_CBACK_INDEX,        //Not Default
        DCI_GROUP3_CADENCE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT32,
    },
    {        // IOT Connection Status reason
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_CONN_STAT_REASON_RAM ),
        &DCI_IOT_CONN_STAT_REASON_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_CONN_STAT_REASON_DEFAULT_ATTRIB ),
        DCI_IOT_CONN_STAT_REASON_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_IOT_CONN_STAT_REASON_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Logging Interval in miliseconds
        DCI_LOG_INTERVAL_TIME_MS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LOG_INTERVAL_TIME_MS_RAM ),
        &DCI_LOG_INTERVAL_TIME_MS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_LOG_INTERVAL_TIME_MS_DEFAULT_ATTRIB ),
        DCI_LOG_INTERVAL_TIME_MS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Change Based Logging
        DCI_CHANGE_BASED_LOGGING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CHANGE_BASED_LOGGING_RAM ),
        &DCI_CHANGE_BASED_LOGGING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CHANGE_BASED_LOGGING_DEFAULT_ATTRIB ),
        DCI_CHANGE_BASED_LOGGING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_CHANGE_BASED_LOGGING_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Manual Trigger for logging
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MANUAL_LOG_TRIGGER_RAM ),
        &DCI_MANUAL_LOG_TRIGGER_LENGTH,
        nullptr,
        DCI_MANUAL_LOG_TRIGGER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_MANUAL_LOG_TRIGGER_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Logging Test 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LOG_TEST_1_RAM ),
        &DCI_LOG_TEST_1_LENGTH,
        nullptr,
        DCI_LOG_TEST_1_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // model short name of the device
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_SNAME_RAM ),
        &DCI_BLE_DEVICE_SNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_SNAME_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_SNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Profile ID
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_PROFILE_ID_RAM ),
        &DCI_BLE_DEVICE_PROFILE_ID_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_PROFILE_ID_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_PROFILE_ID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Vendor ID
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_VENDOR_RAM ),
        &DCI_BLE_DEVICE_VENDOR_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_VENDOR_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_VENDOR_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Family of the BLE device
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_FAMILY_RAM ),
        &DCI_BLE_DEVICE_FAMILY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_FAMILY_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_FAMILY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Role of the BLE device
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_ROLE_RAM ),
        &DCI_BLE_DEVICE_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_ROLE_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Model of the BLE device
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_MODEL_RAM ),
        &DCI_BLE_DEVICE_MODEL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_MODEL_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_MODEL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Device long name
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_LNAME_RAM ),
        &DCI_BLE_DEVICE_LNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_LNAME_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_LNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // HW_Version of BLE device
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_HW_VER_RAM ),
        &DCI_BLE_DEVICE_HW_VER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_HW_VER_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_HW_VER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // SW_Version of BLE device
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_SW_VER_RAM ),
        &DCI_BLE_DEVICE_SW_VER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_SW_VER_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_SW_VER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Automatic Login
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_RAM ),
        &DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // BLE_User_Name
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_USERNAME_RAM ),
        &DCI_BLE_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_USERNAME_DEFAULT_ATTRIB ),
        DCI_BLE_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_USERNAME_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // BLE_Password
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_PASSWORD_RAM ),
        &DCI_BLE_PASSWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_PASSWORD_DEFAULT_ATTRIB ),
        DCI_BLE_PASSWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_PASSWORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Manufacturer name string
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_MANUFACTURER_NAME_RAM ),
        &DCI_BLE_DEVICE_MANUFACTURER_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_MANUFACTURER_NAME_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_MANUFACTURER_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // BLE_Device Serial Number
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_SERIAL_NO_RAM ),
        &DCI_BLE_DEVICE_SERIAL_NO_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_SERIAL_NO_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_SERIAL_NO_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Device Image URL
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_DEVICE_IMAGE_URL_RAM ),
        &DCI_BLE_DEVICE_IMAGE_URL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_DEVICE_IMAGE_URL_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_IMAGE_URL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // BLE Device Bonding info
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_DEVICE_BONDING_INFO_RAM ),
        &DCI_BLE_DEVICE_BONDING_INFO_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_DEVICE_BONDING_INFO_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_BONDING_INFO_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Ble Minimum value for the connection event interval
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_MIN_VAL_CONN_INTERVAL_RAM ),
        &DCI_BLE_MIN_VAL_CONN_INTERVAL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_MIN_VAL_CONN_INTERVAL_DEFAULT_ATTRIB ),
        DCI_BLE_MIN_VAL_CONN_INTERVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_MIN_VAL_CONN_INTERVAL_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Ble Maximum value for the connection event interval
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_MAX_VAL_CONN_INTERVAL_RAM ),
        &DCI_BLE_MAX_VAL_CONN_INTERVAL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_MAX_VAL_CONN_INTERVAL_DEFAULT_ATTRIB ),
        DCI_BLE_MAX_VAL_CONN_INTERVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_MAX_VAL_CONN_INTERVAL_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Slave Latency of the peripheral
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_SLAVE_LATENCY_RAM ),
        &DCI_BLE_SLAVE_LATENCY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_SLAVE_LATENCY_DEFAULT_ATTRIB ),
        DCI_BLE_SLAVE_LATENCY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_SLAVE_LATENCY_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Supervison Timeout of the peripheral
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_SUPERVISION_TIMEOUT_RAM ),
        &DCI_BLE_SUPERVISION_TIMEOUT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_SUPERVISION_TIMEOUT_DEFAULT_ATTRIB ),
        DCI_BLE_SUPERVISION_TIMEOUT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_SUPERVISION_TIMEOUT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Indicate the Active Access Level
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_ACTIVE_ACCESS_LEVEL_RAM ),
        &DCI_BLE_ACTIVE_ACCESS_LEVEL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_ACTIVE_ACCESS_LEVEL_DEFAULT_ATTRIB ),
        DCI_BLE_ACTIVE_ACCESS_LEVEL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ble_Peripheral_Device_passkey
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_DEVICE_PASSKEY_RAM ),
        &DCI_BLE_DEVICE_PASSKEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_DEVICE_PASSKEY_DEFAULT_ATTRIB ),
        DCI_BLE_DEVICE_PASSKEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT32,
    },
    {        // Ble Sample App Input Byte
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_1_RAM ),
        &DCI_BLE_TEST_SETTING_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_1_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BYTE,
    },
    {        // Ble Sample App Input Sint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_2_RAM ),
        &DCI_BLE_TEST_SETTING_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_2_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_SETTING_2_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT8,
    },
    {        // Ble Sample App Input Uint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_TEST_SETTING_3_RAM ),
        &DCI_BLE_TEST_SETTING_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_TEST_SETTING_3_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ble Sample App Input WORD
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_A_RAM ),
        &DCI_BLE_TEST_SETTING_A_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_A_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_A_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_WORD,
    },
    {        // Ble Sample App Input DWORD
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_B_RAM ),
        &DCI_BLE_TEST_SETTING_B_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_B_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_B_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DWORD,
    },
    {        // Ble Sample App Input Uint64
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_C_RAM ),
        &DCI_BLE_TEST_SETTING_C_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_C_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_C_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_SETTING_C_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT64,
    },
    {        // Ble Sample App Input Uint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_TEST_SETTING_D_RAM ),
        &DCI_BLE_TEST_SETTING_D_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_TEST_SETTING_D_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_D_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ble Sample App Input  Uint16
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_E_RAM ),
        &DCI_BLE_TEST_SETTING_E_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_E_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_E_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_SETTING_E_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Ble Sample App Input  Uint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_F_RAM ),
        &DCI_BLE_TEST_SETTING_F_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_F_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_F_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_SETTING_F_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Ble Sample App Input STRING8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_TEST_READING_1_RAM ),
        &DCI_BLE_TEST_READING_1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_TEST_READING_1_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_READING_1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble Sample App Input  BOOL
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_4_RAM ),
        &DCI_BLE_TEST_SETTING_4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_4_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Ble Sample App Input FLOAT
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_READING_2_RAM ),
        &DCI_BLE_TEST_READING_2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_READING_2_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_READING_2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_READING_2_RANGE_INDEX,        //Not Default
        DCI_DTYPE_FLOAT,
    },
    {        // Ble Sample App Input DFLOAT
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_READING_3_RAM ),
        &DCI_BLE_TEST_READING_3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_READING_3_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_READING_3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_READING_3_RANGE_INDEX,        //Not Default
        DCI_DTYPE_DFLOAT,
    },
    {        // Ble Sample App Input Sint16
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_5_RAM ),
        &DCI_BLE_TEST_SETTING_5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_5_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_SETTING_5_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT16,
    },
    {        // Ble Sample App Input Sint32
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_6_RAM ),
        &DCI_BLE_TEST_SETTING_6_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_6_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_6_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_SETTING_6_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT32,
    },
    {        // Ble Sample App Input Uint32
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_SETTING_7_RAM ),
        &DCI_BLE_TEST_SETTING_7_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_SETTING_7_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_SETTING_7_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_SETTING_7_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT32,
    },
    {        // Ble Sample App Input Uint16
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_PRESENT_STATUS_RAM ),
        &DCI_BLE_TEST_PRESENT_STATUS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_PRESENT_STATUS_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_PRESENT_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_PRESENT_STATUS_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Ble Sample App Input Uint32
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_DEVICE_TIME_RAM ),
        &DCI_BLE_TEST_DEVICE_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_DEVICE_TIME_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_DEVICE_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Ble Sample App Input Sint64
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_EVENT_LIST_RAM ),
        &DCI_BLE_TEST_EVENT_LIST_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_EVENT_LIST_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_EVENT_LIST_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_TEST_EVENT_LIST_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT64,
    },
    {        // CR Tasker priority timing list
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_CR_PRIORITY_TIMING_RAM ),
        &DCI_CR_PRIORITY_TIMING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_CR_PRIORITY_TIMING_DEFAULT_ATTRIB ),
        DCI_CR_PRIORITY_TIMING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // OS Tasker priority timing list
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_OS_PRIORITY_TIMING_RAM ),
        &DCI_OS_PRIORITY_TIMING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_OS_PRIORITY_TIMING_DEFAULT_ATTRIB ),
        DCI_OS_PRIORITY_TIMING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // CPU utilization percentage
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CPU_UTILIZATION_RAM ),
        &DCI_CPU_UTILIZATION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CPU_UTILIZATION_DEFAULT_ATTRIB ),
        DCI_CPU_UTILIZATION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Logging event codes
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LOGGING_EVENT_CODE_RAM ),
        &DCI_LOGGING_EVENT_CODE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_LOGGING_EVENT_CODE_DEFAULT_ATTRIB ),
        DCI_LOGGING_EVENT_CODE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_LOGGING_EVENT_CODE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Device state
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DEVICE_STATE_RAM ),
        &DCI_DEVICE_STATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DEVICE_STATE_DEFAULT_ATTRIB ),
        DCI_DEVICE_STATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_DEVICE_STATE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Log Event 101 Test 
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LOG_EVENT_TEST_101_RAM ),
        &DCI_LOG_EVENT_TEST_101_LENGTH,
        nullptr,
        DCI_LOG_EVENT_TEST_101_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Log Event 102 Test 
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LOG_EVENT_TEST_102_RAM ),
        &DCI_LOG_EVENT_TEST_102_LENGTH,
        nullptr,
        DCI_LOG_EVENT_TEST_102_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // NV mem id
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_NV_MEM_ID_RAM ),
        &DCI_NV_MEM_ID_LENGTH,
        nullptr,
        DCI_NV_MEM_ID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_NV_MEM_ID_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // NV parameter
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_NV_PARAMETER_ADDRESS_RAM ),
        &DCI_NV_PARAMETER_ADDRESS_LENGTH,
        nullptr,
        DCI_NV_PARAMETER_ADDRESS_DCID,
        ( 
            0U
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // nv failed operation
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_NV_FAIL_OPERATION_RAM ),
        &DCI_NV_FAIL_OPERATION_LENGTH,
        nullptr,
        DCI_NV_FAIL_OPERATION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_NV_FAIL_OPERATION_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // PROC ID and IMAGE ID
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LOG_PROC_IMAGE_ID_RAM ),
        &DCI_LOG_PROC_IMAGE_ID_LENGTH,
        nullptr,
        DCI_LOG_PROC_IMAGE_ID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_LOG_PROC_IMAGE_ID_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Firmware Version for logging
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_LOG_FIRMWARE_VERSION_RAM ),
        &DCI_LOG_FIRMWARE_VERSION_LENGTH,
        nullptr,
        DCI_LOG_FIRMWARE_VERSION_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Logged user name
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_LOGGED_USERNAME_RAM ),
        &DCI_LOGGED_USERNAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_LOGGED_USERNAME_DEFAULT_ATTRIB ),
        DCI_LOGGED_USERNAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Log IP address
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_LOG_IP_ADDRESS_RAM ),
        &DCI_LOG_IP_ADDRESS_LENGTH,
        nullptr,
        DCI_LOG_IP_ADDRESS_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Modbus TCP enable or disable
        DCI_MODBUS_TCP_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MODBUS_TCP_ENABLE_RAM ),
        &DCI_MODBUS_TCP_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MODBUS_TCP_ENABLE_DEFAULT_ATTRIB ),
        DCI_MODBUS_TCP_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_MODBUS_TCP_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Bacnet IP enable or disable
        DCI_BACNET_IP_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BACNET_IP_ENABLE_RAM ),
        &DCI_BACNET_IP_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BACNET_IP_ENABLE_DEFAULT_ATTRIB ),
        DCI_BACNET_IP_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BACNET_IP_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // Log port number
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LOG_PORT_NUMBER_RAM ),
        &DCI_LOG_PORT_NUMBER_LENGTH,
        nullptr,
        DCI_LOG_PORT_NUMBER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_LOG_PORT_NUMBER_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Ble_Admin_Password
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_ADMIN_PASSWORD_RAM ),
        &DCI_BLE_ADMIN_PASSWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_ADMIN_PASSWORD_DEFAULT_ATTRIB ),
        DCI_BLE_ADMIN_PASSWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_ADMIN_PASSWORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_Observer_Password
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_OBSERVER_PASSWORD_RAM ),
        &DCI_BLE_OBSERVER_PASSWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_OBSERVER_PASSWORD_DEFAULT_ATTRIB ),
        DCI_BLE_OBSERVER_PASSWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_OBSERVER_PASSWORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_Operator_Password
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_OPERATOR_PASSWORD_RAM ),
        &DCI_BLE_OPERATOR_PASSWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_OPERATOR_PASSWORD_DEFAULT_ATTRIB ),
        DCI_BLE_OPERATOR_PASSWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_OPERATOR_PASSWORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_Engineer_Password
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_ENGINEER_PASSWORD_RAM ),
        &DCI_BLE_ENGINEER_PASSWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_ENGINEER_PASSWORD_DEFAULT_ATTRIB ),
        DCI_BLE_ENGINEER_PASSWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_ENGINEER_PASSWORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_User1_Role
        DCI_BLE_USER1_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_USER1_ROLE_RAM ),
        &DCI_BLE_USER1_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_USER1_ROLE_DEFAULT_ATTRIB ),
        DCI_BLE_USER1_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_USER1_ROLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Ble_User1_Name
        DCI_BLE_USER1_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_USER1_NAME_RAM ),
        &DCI_BLE_USER1_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_USER1_NAME_DEFAULT_ATTRIB ),
        DCI_BLE_USER1_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_User1_Password
        DCI_BLE_USER1_PASSWORD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_USER1_PASSWORD_RAM ),
        &DCI_BLE_USER1_PASSWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_USER1_PASSWORD_DEFAULT_ATTRIB ),
        DCI_BLE_USER1_PASSWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_USER1_PASSWORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_User2_Role
        DCI_BLE_USER2_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_USER2_ROLE_RAM ),
        &DCI_BLE_USER2_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_USER2_ROLE_DEFAULT_ATTRIB ),
        DCI_BLE_USER2_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_USER2_ROLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Ble_User2_Name
        DCI_BLE_USER2_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_USER2_NAME_RAM ),
        &DCI_BLE_USER2_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_USER2_NAME_DEFAULT_ATTRIB ),
        DCI_BLE_USER2_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_User2_Password
        DCI_BLE_USER2_PASSWORD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_USER2_PASSWORD_RAM ),
        &DCI_BLE_USER2_PASSWORD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_USER2_PASSWORD_DEFAULT_ATTRIB ),
        DCI_BLE_USER2_PASSWORD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_USER2_PASSWORD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_User3_Role
        DCI_BLE_USER3_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_USER3_ROLE_RAM ),
        &DCI_BLE_USER3_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_USER3_ROLE_DEFAULT_ATTRIB ),
        DCI_BLE_USER3_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_USER3_ROLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Ble_User3_Name
        DCI_BLE_USER3_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_USER3_NAME_RAM ),
        &DCI_BLE_USER3_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_USER3_NAME_DEFAULT_ATTRIB ),
        DCI_BLE_USER3_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Ble_User4_Role
        DCI_BLE_USER4_ROLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_USER4_ROLE_RAM ),
        &DCI_BLE_USER4_ROLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_USER4_ROLE_DEFAULT_ATTRIB ),
        DCI_BLE_USER4_ROLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_BLE_USER4_ROLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Ble_User4_Name
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_USER4_NAME_DEFAULT_ATTRIB )),
        &DCI_BLE_USER4_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_USER4_NAME_DEFAULT_ATTRIB ),
        DCI_BLE_USER4_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Reset Line Number
        DCI_RESET_LINE_NUMBER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_LINE_NUMBER_RAM ),
        &DCI_RESET_LINE_NUMBER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_LINE_NUMBER_DEFAULT_ATTRIB ),
        DCI_RESET_LINE_NUMBER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Reset Function Name
        DCI_RESET_FUNCTION_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_RESET_FUNCTION_NAME_RAM ),
        &DCI_RESET_FUNCTION_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_RESET_FUNCTION_NAME_DEFAULT_ATTRIB ),
        DCI_RESET_FUNCTION_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Reset OS Task Name
        DCI_RESET_OS_TASK_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_RESET_OS_TASK_NAME_RAM ),
        &DCI_RESET_OS_TASK_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_RESET_OS_TASK_NAME_DEFAULT_ATTRIB ),
        DCI_RESET_OS_TASK_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Reset OS Task Priority
        DCI_RESET_OS_TASK_PRIORITY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_OS_TASK_PRIORITY_RAM ),
        &DCI_RESET_OS_TASK_PRIORITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_OS_TASK_PRIORITY_DEFAULT_ATTRIB ),
        DCI_RESET_OS_TASK_PRIORITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Reset OS Stack Size
        DCI_RESET_OS_STACK_SIZE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_OS_STACK_SIZE_RAM ),
        &DCI_RESET_OS_STACK_SIZE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_OS_STACK_SIZE_DEFAULT_ATTRIB ),
        DCI_RESET_OS_STACK_SIZE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Reset CR Task
        DCI_RESET_CR_TASK_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_CR_TASK_RAM ),
        &DCI_RESET_CR_TASK_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_CR_TASK_DEFAULT_ATTRIB ),
        DCI_RESET_CR_TASK_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Reset CR Task Param
        DCI_RESET_CR_TASK_PARAM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_CR_TASK_PARAM_RAM ),
        &DCI_RESET_CR_TASK_PARAM_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_CR_TASK_PARAM_DEFAULT_ATTRIB ),
        DCI_RESET_CR_TASK_PARAM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Reset Cstack Size
        DCI_RESET_CSTACK_SIZE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_CSTACK_SIZE_RAM ),
        &DCI_RESET_CSTACK_SIZE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_CSTACK_SIZE_DEFAULT_ATTRIB ),
        DCI_RESET_CSTACK_SIZE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Reset Highest OS Stack Size
        DCI_RESET_HIGHEST_OS_STACK_SIZE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_HIGHEST_OS_STACK_SIZE_RAM ),
        &DCI_RESET_HIGHEST_OS_STACK_SIZE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_HIGHEST_OS_STACK_SIZE_DEFAULT_ATTRIB ),
        DCI_RESET_HIGHEST_OS_STACK_SIZE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Reset Highest OS Stack Size Task
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_RAM ),
        &DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DEFAULT_ATTRIB ),
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Reset Heap Usage Percentage
        DCI_RESET_HEAP_USAGE_PERCENT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_HEAP_USAGE_PERCENT_RAM ),
        &DCI_RESET_HEAP_USAGE_PERCENT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_HEAP_USAGE_PERCENT_DEFAULT_ATTRIB ),
        DCI_RESET_HEAP_USAGE_PERCENT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Reset Source
        DCI_RESET_SOURCE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_RESET_SOURCE_RAM ),
        &DCI_RESET_SOURCE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_RESET_SOURCE_DEFAULT_ATTRIB ),
        DCI_RESET_SOURCE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_RESET_SOURCE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT32,
    },
    {        // Enable Session Takeover
        DCI_ENABLE_SESSION_TAKEOVER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ENABLE_SESSION_TAKEOVER_RAM ),
        &DCI_ENABLE_SESSION_TAKEOVER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ENABLE_SESSION_TAKEOVER_DEFAULT_ATTRIB ),
        DCI_ENABLE_SESSION_TAKEOVER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Maximum Concurrent Sessions
        DCI_MAX_CONCURRENT_SESSION_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MAX_CONCURRENT_SESSION_RAM ),
        &DCI_MAX_CONCURRENT_SESSION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MAX_CONCURRENT_SESSION_DEFAULT_ATTRIB ),
        DCI_MAX_CONCURRENT_SESSION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_MAX_CONCURRENT_SESSION_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // User account Inactivity timeout
        DCI_USER_INACTIVITY_TIMEOUT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_INACTIVITY_TIMEOUT_RAM ),
        &DCI_USER_INACTIVITY_TIMEOUT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_INACTIVITY_TIMEOUT_DEFAULT_ATTRIB ),
        DCI_USER_INACTIVITY_TIMEOUT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_USER_INACTIVITY_TIMEOUT_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // User Absolute Timeout
        DCI_ABSOLUTE_TIMEOUT_SEC_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_ABSOLUTE_TIMEOUT_SEC_RAM ),
        &DCI_ABSOLUTE_TIMEOUT_SEC_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_ABSOLUTE_TIMEOUT_SEC_DEFAULT_ATTRIB ),
        DCI_ABSOLUTE_TIMEOUT_SEC_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_ABSOLUTE_TIMEOUT_SEC_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT32,
    },
    {        // User Lock Time
        DCI_USER_LOCK_TIME_SEC_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_USER_LOCK_TIME_SEC_RAM ),
        &DCI_USER_LOCK_TIME_SEC_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_USER_LOCK_TIME_SEC_DEFAULT_ATTRIB ),
        DCI_USER_LOCK_TIME_SEC_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_USER_LOCK_TIME_SEC_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // Maximum Failed Login Attempts
        DCI_MAX_FAILED_LOGIN_ATTEMPTS_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MAX_FAILED_LOGIN_ATTEMPTS_RAM ),
        &DCI_MAX_FAILED_LOGIN_ATTEMPTS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MAX_FAILED_LOGIN_ATTEMPTS_DEFAULT_ATTRIB ),
        DCI_MAX_FAILED_LOGIN_ATTEMPTS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_RANGE_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_MAX_FAILED_LOGIN_ATTEMPTS_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT16,
    },
    {        // CORS Origin type
        DCI_CORS_TYPE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CORS_TYPE_RAM ),
        &DCI_CORS_TYPE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CORS_TYPE_DEFAULT_ATTRIB ),
        DCI_CORS_TYPE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_CORS_TYPE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Ble Eddystone URL Advertisement data Input Uint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_EDDYSTONE_URL_RAM ),
        &DCI_BLE_EDDYSTONE_URL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_EDDYSTONE_URL_DEFAULT_ATTRIB ),
        DCI_BLE_EDDYSTONE_URL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ble Transmit Power input Sint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TX_POWER_RAM ),
        &DCI_BLE_TX_POWER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TX_POWER_DEFAULT_ATTRIB ),
        DCI_BLE_TX_POWER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT8,
    },
    {        // Ble iBeacon Major Number input Uint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_IBEACON_MAJOR_NUMBER_RAM ),
        &DCI_BLE_IBEACON_MAJOR_NUMBER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_IBEACON_MAJOR_NUMBER_DEFAULT_ATTRIB ),
        DCI_BLE_IBEACON_MAJOR_NUMBER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ble iBeacon Minor Number input Uint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_IBEACON_MINOR_NUMBER_RAM ),
        &DCI_BLE_IBEACON_MINOR_NUMBER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_IBEACON_MINOR_NUMBER_DEFAULT_ATTRIB ),
        DCI_BLE_IBEACON_MINOR_NUMBER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ble iBeacon uuid input Uint8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_IBEACON_128_BIT_UUID_RAM ),
        &DCI_BLE_IBEACON_128_BIT_UUID_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BLE_IBEACON_128_BIT_UUID_DEFAULT_ATTRIB ),
        DCI_BLE_IBEACON_128_BIT_UUID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Ble Find Target Device 
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_RAM ),
        &DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_DEFAULT_ATTRIB ),
        DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // mDNS lwm2m Service Discovery Enable
        DCI_MDNS_LWM2M_SERVICE_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MDNS_LWM2M_SERVICE_ENABLE_RAM ),
        &DCI_MDNS_LWM2M_SERVICE_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MDNS_LWM2M_SERVICE_ENABLE_DEFAULT_ATTRIB ),
        DCI_MDNS_LWM2M_SERVICE_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_MDNS_LWM2M_SERVICE_ENABLE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // mDNS Server 1 Hostname
        DCI_MDNS_SERVER1_NAME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MDNS_SERVER1_NAME_RAM ),
        &DCI_MDNS_SERVER1_NAME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MDNS_SERVER1_NAME_DEFAULT_ATTRIB ),
        DCI_MDNS_SERVER1_NAME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // mDNS Server 1 IP Address
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_MDNS_SERVER1_IP_ADDR_RAM ),
        &DCI_MDNS_SERVER1_IP_ADDR_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_MDNS_SERVER1_IP_ADDR_DEFAULT_ATTRIB ),
        DCI_MDNS_SERVER1_IP_ADDR_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_MDNS_SERVER1_IP_ADDR_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // mDNS Server 1 Service Port
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_MDNS_SERVER1_PORT_RAM ),
        &DCI_MDNS_SERVER1_PORT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_MDNS_SERVER1_PORT_DEFAULT_ATTRIB ),
        DCI_MDNS_SERVER1_PORT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // J1939 Message Example DCI_1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX1_RAM ),
        &DCI_J1939_MSG_EX1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX1_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // J1939 Message Example DCI_2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX2_RAM ),
        &DCI_J1939_MSG_EX2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX2_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // J1939 Message Example DCI_3
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX3_RAM ),
        &DCI_J1939_MSG_EX3_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX3_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX3_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // J1939 Message Example DCI_4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX4_RAM ),
        &DCI_J1939_MSG_EX4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX4_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // J1939 Message Example DCI_5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX5_RAM ),
        &DCI_J1939_MSG_EX5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX5_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // J1939 Message Example DCI_6
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX6_RAM ),
        &DCI_J1939_MSG_EX6_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX6_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX6_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // J1939 Message Example DCI_7
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX7_RAM ),
        &DCI_J1939_MSG_EX7_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX7_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX7_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // J1939 Message Example DCI_8
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX8_RAM ),
        &DCI_J1939_MSG_EX8_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX8_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX8_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // J1939 Message Example DCI_9
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX9_RAM ),
        &DCI_J1939_MSG_EX9_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX9_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX9_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // J1939 Message Example DCI_10
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_J1939_MSG_EX10_RAM ),
        &DCI_J1939_MSG_EX10_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_J1939_MSG_EX10_DEFAULT_ATTRIB ),
        DCI_J1939_MSG_EX10_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Device Password Scheme
        DCI_DEVICE_PWD_SETTING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DEVICE_PWD_SETTING_RAM ),
        &DCI_DEVICE_PWD_SETTING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DEVICE_PWD_SETTING_DEFAULT_ATTRIB ),
        DCI_DEVICE_PWD_SETTING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_DEVICE_PWD_SETTING_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Open Source License
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_LICENSE_INFORMATION_DEFAULT_ATTRIB )),
        &DCI_LICENSE_INFORMATION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_LICENSE_INFORMATION_DEFAULT_ATTRIB ),
        DCI_LICENSE_INFORMATION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Device Default Password
        DCI_DEVICE_DEFAULT_PWD_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_DEVICE_DEFAULT_PWD_RAM ),
        &DCI_DEVICE_DEFAULT_PWD_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_DEVICE_DEFAULT_PWD_DEFAULT_ATTRIB ),
        DCI_DEVICE_DEFAULT_PWD_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_DEVICE_DEFAULT_PWD_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Preferred Language Setting
        DCI_LANG_PREF_SETTING_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LANG_PREF_SETTING_RAM ),
        &DCI_LANG_PREF_SETTING_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_LANG_PREF_SETTING_DEFAULT_ATTRIB ),
        DCI_LANG_PREF_SETTING_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_LANG_PREF_SETTING_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Common Language Preferrence
        DCI_COMMON_LANG_PREF_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_COMMON_LANG_PREF_RAM ),
        &DCI_COMMON_LANG_PREF_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_COMMON_LANG_PREF_DEFAULT_ATTRIB ),
        DCI_COMMON_LANG_PREF_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Language firmware Version Number
        DCI_LANG_FIRM_VER_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LANG_FIRM_VER_NUM_RAM ),
        &DCI_LANG_FIRM_VER_NUM_LENGTH,
        nullptr,
        DCI_LANG_FIRM_VER_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // User who upgraded last language firmware
        DCI_LAST_LANG_FIRM_UPGRADE_USER_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_LAST_LANG_FIRM_UPGRADE_USER_RAM ),
        &DCI_LAST_LANG_FIRM_UPGRADE_USER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_LAST_LANG_FIRM_UPGRADE_USER_DEFAULT_ATTRIB ),
        DCI_LAST_LANG_FIRM_UPGRADE_USER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Language Firmware Upgrade Epoch Time
        DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_RAM ),
        &DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_DEFAULT_ATTRIB ),
        DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // BLE Test Log
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BLE_LOG_RAM ),
        &DCI_BLE_LOG_LENGTH,
        nullptr,
        DCI_BLE_LOG_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
            0U
        ),
        DCI_BLE_LOG_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // BLE Test Log Index
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_LOG_INDEX_RAM ),
        &DCI_BLE_LOG_INDEX_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_LOG_INDEX_DEFAULT_ATTRIB ),
        DCI_BLE_LOG_INDEX_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_BLE_LOG_INDEX_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // Test Counter for logging data
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_BLE_TEST_COUNTER_RAM ),
        &DCI_BLE_TEST_COUNTER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_BLE_TEST_COUNTER_DEFAULT_ATTRIB ),
        DCI_BLE_TEST_COUNTER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Data Log
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DATA_LOG_DEFAULT_ATTRIB )),
        &DCI_DATA_LOG_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DATA_LOG_DEFAULT_ATTRIB ),
        DCI_DATA_LOG_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Event Log
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EVENT_LOG_DEFAULT_ATTRIB )),
        &DCI_EVENT_LOG_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EVENT_LOG_DEFAULT_ATTRIB ),
        DCI_EVENT_LOG_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Audit-Power Log
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_POWER_LOG_DEFAULT_ATTRIB )),
        &DCI_AUDIT_POWER_LOG_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_POWER_LOG_DEFAULT_ATTRIB ),
        DCI_AUDIT_POWER_LOG_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Audit-Fw-Update Log
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_FW_UPGRADE_LOG_DEFAULT_ATTRIB )),
        &DCI_AUDIT_FW_UPGRADE_LOG_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_FW_UPGRADE_LOG_DEFAULT_ATTRIB ),
        DCI_AUDIT_FW_UPGRADE_LOG_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Audit-User Log
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_USER_LOG_DEFAULT_ATTRIB )),
        &DCI_AUDIT_USER_LOG_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_USER_LOG_DEFAULT_ATTRIB ),
        DCI_AUDIT_USER_LOG_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Audit-Config Log
        DCI_DATA_NO_NV_MEM_DATA,
        const_cast<DCI_RAM_DATA_TD *>( reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_CONFIG_LOG_DEFAULT_ATTRIB )),
        &DCI_AUDIT_CONFIG_LOG_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_CONFIG_LOG_DEFAULT_ATTRIB ),
        DCI_AUDIT_CONFIG_LOG_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK )
        ),
        ( 
            0U
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Device Up Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DEVICE_UP_TIME_SEC_RAM ),
        &DCI_DEVICE_UP_TIME_SEC_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DEVICE_UP_TIME_SEC_DEFAULT_ATTRIB ),
        DCI_DEVICE_UP_TIME_SEC_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // PTP enable or disable
        DCI_PTP_ENABLE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PTP_ENABLE_RAM ),
        &DCI_PTP_ENABLE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PTP_ENABLE_DEFAULT_ATTRIB ),
        DCI_PTP_ENABLE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_PTP_ENABLE_RANGE_INDEX,        //Not Default
        DCI_DTYPE_BOOL,
    },
    {        // PTP Seconds timestamp
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PTP_SECONDS_TIME_RAM ),
        &DCI_PTP_SECONDS_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PTP_SECONDS_TIME_DEFAULT_ATTRIB ),
        DCI_PTP_SECONDS_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // PTP Nanoseconds timestamp
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_PTP_NANOSECONDS_TIME_RAM ),
        &DCI_PTP_NANOSECONDS_TIME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_PTP_NANOSECONDS_TIME_DEFAULT_ATTRIB ),
        DCI_PTP_NANOSECONDS_TIME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // VLAN priority
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_PRI1_RAM ),
        &DCI_IEC61850_VLAN_PRI1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_PRI1_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_PRI1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // VLAN VID
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_VID1_RAM ),
        &DCI_IEC61850_VLAN_VID1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_VID1_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_VID1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // VLAN application ID
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_APP_ID1_RAM ),
        &DCI_IEC61850_VLAN_APP_ID1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_APP_ID1_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_APP_ID1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT16,
    },
    {        // Mac address 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_MAC_ADD_ID1_RAM ),
        &DCI_IEC61850_MAC_ADD_ID1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_MAC_ADD_ID1_DEFAULT_ATTRIB ),
        DCI_IEC61850_MAC_ADD_ID1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // GOOSE ID 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_GOOSE_ID1_RAM ),
        &DCI_IEC61850_GOOSE_ID1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_GOOSE_ID1_DEFAULT_ATTRIB ),
        DCI_IEC61850_GOOSE_ID1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // CB Ref Name 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_CB_REF_NAME1_RAM ),
        &DCI_IEC61850_CB_REF_NAME1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_CB_REF_NAME1_DEFAULT_ATTRIB ),
        DCI_IEC61850_CB_REF_NAME1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Data set name 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_DATA_SET_NAME1_RAM ),
        &DCI_IEC61850_DATA_SET_NAME1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_DATA_SET_NAME1_DEFAULT_ATTRIB ),
        DCI_IEC61850_DATA_SET_NAME1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // ConfRev 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_CONFIG_REV1_RAM ),
        &DCI_IEC61850_CONFIG_REV1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_CONFIG_REV1_DEFAULT_ATTRIB ),
        DCI_IEC61850_CONFIG_REV1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // ndsCommisioning 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_NDS_COM1_RAM ),
        &DCI_IEC61850_NDS_COM1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_NDS_COM1_DEFAULT_ATTRIB ),
        DCI_IEC61850_NDS_COM1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Test 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_TEST1_RAM ),
        &DCI_IEC61850_TEST1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_TEST1_DEFAULT_ATTRIB ),
        DCI_IEC61850_TEST1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Number of Data set entries 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_NUM_DATA_SET_ENTRIES1_RAM ),
        &DCI_IEC61850_NUM_DATA_SET_ENTRIES1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_NUM_DATA_SET_ENTRIES1_DEFAULT_ATTRIB ),
        DCI_IEC61850_NUM_DATA_SET_ENTRIES1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Pin 1 Binary Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND1_STVAL_RAM ),
        &DCI_IEC61850_IND1_STVAL_LENGTH,
        nullptr,
        DCI_IEC61850_IND1_STVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND1_STVAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Pin 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND1_Q_RAM ),
        &DCI_IEC61850_IND1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_IND1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Pin 1 Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND1_T_RAM ),
        &DCI_IEC61850_IND1_T_LENGTH,
        nullptr,
        DCI_IEC61850_IND1_T_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND1_T_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // Pin 2 Binary Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND2_STVAL_RAM ),
        &DCI_IEC61850_IND2_STVAL_LENGTH,
        nullptr,
        DCI_IEC61850_IND2_STVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND2_STVAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Pin 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND2_Q_RAM ),
        &DCI_IEC61850_IND2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_IND2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Pin 2 Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND2_T_RAM ),
        &DCI_IEC61850_IND2_T_LENGTH,
        nullptr,
        DCI_IEC61850_IND2_T_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND2_T_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // VLAN priority
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_PRI2_RAM ),
        &DCI_IEC61850_VLAN_PRI2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_PRI2_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_PRI2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // VLAN VID
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_VID2_RAM ),
        &DCI_IEC61850_VLAN_VID2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_VID2_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_VID2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // VLAN application ID
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_APP_ID2_RAM ),
        &DCI_IEC61850_VLAN_APP_ID2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_APP_ID2_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_APP_ID2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT16,
    },
    {        // Mac address 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_MAC_ADD_ID2_RAM ),
        &DCI_IEC61850_MAC_ADD_ID2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_MAC_ADD_ID2_DEFAULT_ATTRIB ),
        DCI_IEC61850_MAC_ADD_ID2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // GOOSE ID 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_GOOSE_ID2_RAM ),
        &DCI_IEC61850_GOOSE_ID2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_GOOSE_ID2_DEFAULT_ATTRIB ),
        DCI_IEC61850_GOOSE_ID2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // CB Ref Name 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_CB_REF_NAME2_RAM ),
        &DCI_IEC61850_CB_REF_NAME2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_CB_REF_NAME2_DEFAULT_ATTRIB ),
        DCI_IEC61850_CB_REF_NAME2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Data set name 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_DATA_SET_NAME2_RAM ),
        &DCI_IEC61850_DATA_SET_NAME2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_DATA_SET_NAME2_DEFAULT_ATTRIB ),
        DCI_IEC61850_DATA_SET_NAME2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // ConfRev 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_CONFIG_REV2_RAM ),
        &DCI_IEC61850_CONFIG_REV2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_CONFIG_REV2_DEFAULT_ATTRIB ),
        DCI_IEC61850_CONFIG_REV2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // ndsCommisioning 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_NDS_COM2_RAM ),
        &DCI_IEC61850_NDS_COM2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_NDS_COM2_DEFAULT_ATTRIB ),
        DCI_IEC61850_NDS_COM2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Test 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_TEST2_RAM ),
        &DCI_IEC61850_TEST2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_TEST2_DEFAULT_ATTRIB ),
        DCI_IEC61850_TEST2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Number of Data set entries 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_NUM_DATA_SET_ENTRIES2_RAM ),
        &DCI_IEC61850_NUM_DATA_SET_ENTRIES2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_NUM_DATA_SET_ENTRIES2_DEFAULT_ATTRIB ),
        DCI_IEC61850_NUM_DATA_SET_ENTRIES2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Pin 3 Binary Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND3_STVAL_RAM ),
        &DCI_IEC61850_IND3_STVAL_LENGTH,
        nullptr,
        DCI_IEC61850_IND3_STVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND3_STVAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Pin 3 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND3_Q_RAM ),
        &DCI_IEC61850_IND3_Q_LENGTH,
        nullptr,
        DCI_IEC61850_IND3_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND3_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Pin 3 Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND3_T_RAM ),
        &DCI_IEC61850_IND3_T_LENGTH,
        nullptr,
        DCI_IEC61850_IND3_T_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND3_T_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // Pin 4 Binary Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND4_STVAL_RAM ),
        &DCI_IEC61850_IND4_STVAL_LENGTH,
        nullptr,
        DCI_IEC61850_IND4_STVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND4_STVAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Pin 4 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND4_Q_RAM ),
        &DCI_IEC61850_IND4_Q_LENGTH,
        nullptr,
        DCI_IEC61850_IND4_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND4_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Pin 4 Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_IND4_T_RAM ),
        &DCI_IEC61850_IND4_T_LENGTH,
        nullptr,
        DCI_IEC61850_IND4_T_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_IND4_T_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // VLAN priority 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_PRI4_RAM ),
        &DCI_IEC61850_VLAN_PRI4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_PRI4_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_PRI4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // VLAN VID 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_VID4_RAM ),
        &DCI_IEC61850_VLAN_VID4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_VID4_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_VID4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // VLAN application ID 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_APP_ID4_RAM ),
        &DCI_IEC61850_VLAN_APP_ID4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_APP_ID4_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_APP_ID4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT16,
    },
    {        // Mac address 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_MAC_ADD_ID4_RAM ),
        &DCI_IEC61850_MAC_ADD_ID4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_MAC_ADD_ID4_DEFAULT_ATTRIB ),
        DCI_IEC61850_MAC_ADD_ID4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // GOOSE ID 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_GOOSE_ID4_RAM ),
        &DCI_IEC61850_GOOSE_ID4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_GOOSE_ID4_DEFAULT_ATTRIB ),
        DCI_IEC61850_GOOSE_ID4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // CB Ref Name 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_CB_REF_NAME4_RAM ),
        &DCI_IEC61850_CB_REF_NAME4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_CB_REF_NAME4_DEFAULT_ATTRIB ),
        DCI_IEC61850_CB_REF_NAME4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Data set name 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_DATA_SET_NAME4_RAM ),
        &DCI_IEC61850_DATA_SET_NAME4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_DATA_SET_NAME4_DEFAULT_ATTRIB ),
        DCI_IEC61850_DATA_SET_NAME4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // ConfRev 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_CONFIG_REV4_RAM ),
        &DCI_IEC61850_CONFIG_REV4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_CONFIG_REV4_DEFAULT_ATTRIB ),
        DCI_IEC61850_CONFIG_REV4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // ndsCommisioning 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_NDS_COM4_RAM ),
        &DCI_IEC61850_NDS_COM4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_NDS_COM4_DEFAULT_ATTRIB ),
        DCI_IEC61850_NDS_COM4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Test 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_TEST4_RAM ),
        &DCI_IEC61850_TEST4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_TEST4_DEFAULT_ATTRIB ),
        DCI_IEC61850_TEST4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Number of Data set entries 4
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_NUM_DATA_SET_ENTRIES4_RAM ),
        &DCI_IEC61850_NUM_DATA_SET_ENTRIES4_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_NUM_DATA_SET_ENTRIES4_DEFAULT_ATTRIB ),
        DCI_IEC61850_NUM_DATA_SET_ENTRIES4_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Relay 1 Binary Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM1_STVAL_RAM ),
        &DCI_IEC61850_ALM1_STVAL_LENGTH,
        nullptr,
        DCI_IEC61850_ALM1_STVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM1_STVAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Relay 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM1_Q_RAM ),
        &DCI_IEC61850_ALM1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_ALM1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Relay 1 Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM1_T_RAM ),
        &DCI_IEC61850_ALM1_T_LENGTH,
        nullptr,
        DCI_IEC61850_ALM1_T_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM1_T_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // Relay 2 Binary Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM2_STVAL_RAM ),
        &DCI_IEC61850_ALM2_STVAL_LENGTH,
        nullptr,
        DCI_IEC61850_ALM2_STVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM2_STVAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Relay 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM2_Q_RAM ),
        &DCI_IEC61850_ALM2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_ALM2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Relay 2 Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM2_T_RAM ),
        &DCI_IEC61850_ALM2_T_LENGTH,
        nullptr,
        DCI_IEC61850_ALM2_T_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM2_T_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // VLAN priority 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_PRI5_RAM ),
        &DCI_IEC61850_VLAN_PRI5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_PRI5_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_PRI5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // VLAN VID 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_VID5_RAM ),
        &DCI_IEC61850_VLAN_VID5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_VID5_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_VID5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // VLAN application ID 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_VLAN_APP_ID5_RAM ),
        &DCI_IEC61850_VLAN_APP_ID5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_VLAN_APP_ID5_DEFAULT_ATTRIB ),
        DCI_IEC61850_VLAN_APP_ID5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT16,
    },
    {        // Mac address 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_MAC_ADD_ID5_RAM ),
        &DCI_IEC61850_MAC_ADD_ID5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_MAC_ADD_ID5_DEFAULT_ATTRIB ),
        DCI_IEC61850_MAC_ADD_ID5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // GOOSE ID 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_GOOSE_ID5_RAM ),
        &DCI_IEC61850_GOOSE_ID5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_GOOSE_ID5_DEFAULT_ATTRIB ),
        DCI_IEC61850_GOOSE_ID5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // CB Ref Name 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_CB_REF_NAME5_RAM ),
        &DCI_IEC61850_CB_REF_NAME5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_CB_REF_NAME5_DEFAULT_ATTRIB ),
        DCI_IEC61850_CB_REF_NAME5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Data set name 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_DATA_SET_NAME5_RAM ),
        &DCI_IEC61850_DATA_SET_NAME5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_DATA_SET_NAME5_DEFAULT_ATTRIB ),
        DCI_IEC61850_DATA_SET_NAME5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // ConfRev 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_CONFIG_REV5_RAM ),
        &DCI_IEC61850_CONFIG_REV5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_CONFIG_REV5_DEFAULT_ATTRIB ),
        DCI_IEC61850_CONFIG_REV5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // ndsCommisioning 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_NDS_COM5_RAM ),
        &DCI_IEC61850_NDS_COM5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_NDS_COM5_DEFAULT_ATTRIB ),
        DCI_IEC61850_NDS_COM5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Test 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_TEST5_RAM ),
        &DCI_IEC61850_TEST5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_TEST5_DEFAULT_ATTRIB ),
        DCI_IEC61850_TEST5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Number of Data set entries 5
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_NUM_DATA_SET_ENTRIES5_RAM ),
        &DCI_IEC61850_NUM_DATA_SET_ENTRIES5_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_NUM_DATA_SET_ENTRIES5_DEFAULT_ATTRIB ),
        DCI_IEC61850_NUM_DATA_SET_ENTRIES5_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Relay 5 Binary Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM5_STVAL_RAM ),
        &DCI_IEC61850_ALM5_STVAL_LENGTH,
        nullptr,
        DCI_IEC61850_ALM5_STVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM5_STVAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Relay 5 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM5_Q_RAM ),
        &DCI_IEC61850_ALM5_Q_LENGTH,
        nullptr,
        DCI_IEC61850_ALM5_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM5_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Relay 5 Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM5_T_RAM ),
        &DCI_IEC61850_ALM5_T_LENGTH,
        nullptr,
        DCI_IEC61850_ALM5_T_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM5_T_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // Relay 6 Binary Status
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM6_STVAL_RAM ),
        &DCI_IEC61850_ALM6_STVAL_LENGTH,
        nullptr,
        DCI_IEC61850_ALM6_STVAL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM6_STVAL_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Relay 6 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM6_Q_RAM ),
        &DCI_IEC61850_ALM6_Q_LENGTH,
        nullptr,
        DCI_IEC61850_ALM6_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM6_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Relay 6 Time
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_ALM6_T_RAM ),
        &DCI_IEC61850_ALM6_T_LENGTH,
        nullptr,
        DCI_IEC61850_ALM6_T_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_ALM6_T_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // SAV_VLAN priority 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_VLAN_PRI1_RAM ),
        &DCI_IEC61850_SAV_VLAN_PRI1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_VLAN_PRI1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_VLAN_PRI1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SAV_VLAN VID 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_VLAN_VID1_RAM ),
        &DCI_IEC61850_SAV_VLAN_VID1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_VLAN_VID1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_VLAN_VID1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_VLAN application ID 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_VLAN_APP_ID1_RAM ),
        &DCI_IEC61850_SAV_VLAN_APP_ID1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_VLAN_APP_ID1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_VLAN_APP_ID1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_Mac address 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_MAC_ADD_ID1_RAM ),
        &DCI_IEC61850_SAV_MAC_ADD_ID1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_SAV_MAC_ADD_ID1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_MAC_ADD_ID1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SAV ID 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_ID1_RAM ),
        &DCI_IEC61850_SAV_ID1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_SAV_ID1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_ID1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // SAV_CB Ref Name 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_CB_REF_NAME1_RAM ),
        &DCI_IEC61850_SAV_CB_REF_NAME1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_SAV_CB_REF_NAME1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_CB_REF_NAME1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // SAV_Data set name 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_DATA_SET_NAME1_RAM ),
        &DCI_IEC61850_SAV_DATA_SET_NAME1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_SAV_DATA_SET_NAME1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_DATA_SET_NAME1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // SAV_ConfRev 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_CONFIG_REV1_RAM ),
        &DCI_IEC61850_SAV_CONFIG_REV1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_CONFIG_REV1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_CONFIG_REV1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_Sample Sync 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_SMP_SYNC1_RAM ),
        &DCI_IEC61850_SAV_SMP_SYNC1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_SMP_SYNC1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_SMP_SYNC1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SAV_Sample Rate 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_SMP_RATE1_RAM ),
        &DCI_IEC61850_SAV_SMP_RATE1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_SMP_RATE1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_SMP_RATE1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_Sample Mode 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_SMP_MODE1_RAM ),
        &DCI_IEC61850_SAV_SMP_MODE1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_SMP_MODE1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_SMP_MODE1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT16,
    },
    {        // SAV_Num Cycles Per Sec 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC1_RAM ),
        &DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_Num ASDU 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_NUM_ASDU1_RAM ),
        &DCI_IEC61850_SAV_NUM_ASDU1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_NUM_ASDU1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_NUM_ASDU1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_Number of Dataset Entries 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES1_RAM ),
        &DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES1_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SAV_PhaseA Amp 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHA_AMP1_RAM ),
        &DCI_IEC61850_SAV_PHA_AMP1_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHA_AMP1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHA_AMP1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseA Amp 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHA_AMP1_Q_RAM ),
        &DCI_IEC61850_SAV_PHA_AMP1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHA_AMP1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHA_AMP1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseB Amp 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHB_AMP1_RAM ),
        &DCI_IEC61850_SAV_PHB_AMP1_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHB_AMP1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHB_AMP1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseB Amp 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHB_AMP1_Q_RAM ),
        &DCI_IEC61850_SAV_PHB_AMP1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHB_AMP1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHB_AMP1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseC Amp 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHC_AMP1_RAM ),
        &DCI_IEC61850_SAV_PHC_AMP1_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHC_AMP1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHC_AMP1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseC Amp 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHC_AMP1_Q_RAM ),
        &DCI_IEC61850_SAV_PHC_AMP1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHC_AMP1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHC_AMP1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseN Amp 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHN_AMP1_RAM ),
        &DCI_IEC61850_SAV_PHN_AMP1_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHN_AMP1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHN_AMP1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseN Amp 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHN_AMP1_Q_RAM ),
        &DCI_IEC61850_SAV_PHN_AMP1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHN_AMP1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHN_AMP1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseA Volt 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHA_VOLT1_RAM ),
        &DCI_IEC61850_SAV_PHA_VOLT1_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHA_VOLT1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHA_VOLT1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseA Volt 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHA_VOLT1_Q_RAM ),
        &DCI_IEC61850_SAV_PHA_VOLT1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHA_VOLT1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHA_VOLT1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseB Volt 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHB_VOLT1_RAM ),
        &DCI_IEC61850_SAV_PHB_VOLT1_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHB_VOLT1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHB_VOLT1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseB Volt 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHB_VOLT1_Q_RAM ),
        &DCI_IEC61850_SAV_PHB_VOLT1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHB_VOLT1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHB_VOLT1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseC Volt 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHC_VOLT1_RAM ),
        &DCI_IEC61850_SAV_PHC_VOLT1_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHC_VOLT1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHC_VOLT1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseC Volt 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHC_VOLT1_Q_RAM ),
        &DCI_IEC61850_SAV_PHC_VOLT1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHC_VOLT1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHC_VOLT1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseN Volt 1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHN_VOLT1_RAM ),
        &DCI_IEC61850_SAV_PHN_VOLT1_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHN_VOLT1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHN_VOLT1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseN Volt 1 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHN_VOLT1_Q_RAM ),
        &DCI_IEC61850_SAV_PHN_VOLT1_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHN_VOLT1_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHN_VOLT1_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_VLAN priority 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_VLAN_PRI2_RAM ),
        &DCI_IEC61850_SAV_VLAN_PRI2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_VLAN_PRI2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_VLAN_PRI2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SAV_VLAN VID 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_VLAN_VID2_RAM ),
        &DCI_IEC61850_SAV_VLAN_VID2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_VLAN_VID2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_VLAN_VID2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_VLAN application ID 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_VLAN_APP_ID2_RAM ),
        &DCI_IEC61850_SAV_VLAN_APP_ID2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_VLAN_APP_ID2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_VLAN_APP_ID2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_Mac address 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_MAC_ADD_ID2_RAM ),
        &DCI_IEC61850_SAV_MAC_ADD_ID2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_SAV_MAC_ADD_ID2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_MAC_ADD_ID2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // SAV ID 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_ID2_RAM ),
        &DCI_IEC61850_SAV_ID2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_SAV_ID2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_ID2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // SAV_CB Ref Name 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_CB_REF_NAME2_RAM ),
        &DCI_IEC61850_SAV_CB_REF_NAME2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_SAV_CB_REF_NAME2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_CB_REF_NAME2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // SAV_Data set name 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_DATA_SET_NAME2_RAM ),
        &DCI_IEC61850_SAV_DATA_SET_NAME2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IEC61850_SAV_DATA_SET_NAME2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_DATA_SET_NAME2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // SAV_ConfRev 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_CONFIG_REV2_RAM ),
        &DCI_IEC61850_SAV_CONFIG_REV2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_CONFIG_REV2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_CONFIG_REV2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SAV_Sample Sync 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_SMP_SYNC2_RAM ),
        &DCI_IEC61850_SAV_SMP_SYNC2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_SMP_SYNC2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_SMP_SYNC2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_Sample Rate 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_SMP_RATE2_RAM ),
        &DCI_IEC61850_SAV_SMP_RATE2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_SMP_RATE2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_SMP_RATE2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT16,
    },
    {        // SAV_Sample Mode 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_SMP_MODE2_RAM ),
        &DCI_IEC61850_SAV_SMP_MODE2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_SMP_MODE2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_SMP_MODE2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_Num Cycles Per Sec 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC2_RAM ),
        &DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_NUM_CYCLES_PER_SEC2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_Num ASDU 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_NUM_ASDU2_RAM ),
        &DCI_IEC61850_SAV_NUM_ASDU2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_NUM_ASDU2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_NUM_ASDU2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SAV_Number of Dataset Entries 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES2_RAM ),
        &DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES2_DEFAULT_ATTRIB ),
        DCI_IEC61850_SAV_NUM_DATA_SET_ENTRIES2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SAV_PhaseA Amp2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHA_AMP2_RAM ),
        &DCI_IEC61850_SAV_PHA_AMP2_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHA_AMP2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHA_AMP2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseA Amp 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHA_AMP2_Q_RAM ),
        &DCI_IEC61850_SAV_PHA_AMP2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHA_AMP2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHA_AMP2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseB Amp 2 
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHB_AMP2_RAM ),
        &DCI_IEC61850_SAV_PHB_AMP2_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHB_AMP2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHB_AMP2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseB Amp 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHB_AMP2_Q_RAM ),
        &DCI_IEC61850_SAV_PHB_AMP2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHB_AMP2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHB_AMP2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseC Amp 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHC_AMP2_RAM ),
        &DCI_IEC61850_SAV_PHC_AMP2_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHC_AMP2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHC_AMP2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseC Amp 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHC_AMP2_Q_RAM ),
        &DCI_IEC61850_SAV_PHC_AMP2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHC_AMP2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHC_AMP2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseN Amp 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHN_AMP2_RAM ),
        &DCI_IEC61850_SAV_PHN_AMP2_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHN_AMP2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHN_AMP2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseN Amp 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHN_AMP2_Q_RAM ),
        &DCI_IEC61850_SAV_PHN_AMP2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHN_AMP2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHN_AMP2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseA Volt 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHA_VOLT2_RAM ),
        &DCI_IEC61850_SAV_PHA_VOLT2_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHA_VOLT2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHA_VOLT2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseA Volt 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHA_VOLT2_Q_RAM ),
        &DCI_IEC61850_SAV_PHA_VOLT2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHA_VOLT2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHA_VOLT2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseB Volt 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHB_VOLT2_RAM ),
        &DCI_IEC61850_SAV_PHB_VOLT2_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHB_VOLT2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHB_VOLT2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseB Volt 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHB_VOLT2_Q_RAM ),
        &DCI_IEC61850_SAV_PHB_VOLT2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHB_VOLT2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHB_VOLT2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseC Volt 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHC_VOLT2_RAM ),
        &DCI_IEC61850_SAV_PHC_VOLT2_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHC_VOLT2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHC_VOLT2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseC Volt 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHC_VOLT2_Q_RAM ),
        &DCI_IEC61850_SAV_PHC_VOLT2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHC_VOLT2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHC_VOLT2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SAV_PhaseN Volt 2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHN_VOLT2_RAM ),
        &DCI_IEC61850_SAV_PHN_VOLT2_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHN_VOLT2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHN_VOLT2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SAV_PhaseN Volt 2 Quality
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IEC61850_SAV_PHN_VOLT2_Q_RAM ),
        &DCI_IEC61850_SAV_PHN_VOLT2_Q_LENGTH,
        nullptr,
        DCI_IEC61850_SAV_PHN_VOLT2_Q_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IEC61850_SAV_PHN_VOLT2_Q_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // Client AP SSID
        DCI_CLIENT_AP_SSID_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_CLIENT_AP_SSID_RAM ),
        &DCI_CLIENT_AP_SSID_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_CLIENT_AP_SSID_DEFAULT_ATTRIB ),
        DCI_CLIENT_AP_SSID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_CLIENT_AP_SSID_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Client AP Passphrase
        DCI_CLIENT_AP_PASSPHRASE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_CLIENT_AP_PASSPHRASE_RAM ),
        &DCI_CLIENT_AP_PASSPHRASE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_CLIENT_AP_PASSPHRASE_DEFAULT_ATTRIB ),
        DCI_CLIENT_AP_PASSPHRASE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_CLIENT_AP_PASSPHRASE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Client AP Security
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CLIENT_AP_SECURITY_RAM ),
        &DCI_CLIENT_AP_SECURITY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CLIENT_AP_SECURITY_DEFAULT_ATTRIB ),
        DCI_CLIENT_AP_SECURITY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_CLIENT_AP_SECURITY_RANGE_INDEX,        //Not Default
        DCI_DTYPE_SINT32,
    },
    {        // Client AP Channel
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CLIENT_AP_CHANNEL_RAM ),
        &DCI_CLIENT_AP_CHANNEL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CLIENT_AP_CHANNEL_DEFAULT_ATTRIB ),
        DCI_CLIENT_AP_CHANNEL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Client AP Band
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CLIENT_AP_BAND_RAM ),
        &DCI_CLIENT_AP_BAND_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CLIENT_AP_BAND_DEFAULT_ATTRIB ),
        DCI_CLIENT_AP_BAND_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_ENUM_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        DCI_CLIENT_AP_BAND_RANGE_INDEX,        //Not Default
        DCI_DTYPE_UINT8,
    },
    {        // Client AP Retry Count
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_CLIENT_AP_RETRY_COUNT_RAM ),
        &DCI_CLIENT_AP_RETRY_COUNT_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_CLIENT_AP_RETRY_COUNT_DEFAULT_ATTRIB ),
        DCI_CLIENT_AP_RETRY_COUNT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Source AP SSID
        DCI_SOURCE_AP_SSID_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_SOURCE_AP_SSID_RAM ),
        &DCI_SOURCE_AP_SSID_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_SOURCE_AP_SSID_DEFAULT_ATTRIB ),
        DCI_SOURCE_AP_SSID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_SOURCE_AP_SSID_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Source AP Passphrase
        DCI_SOURCE_AP_PASSPHRASE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_SOURCE_AP_PASSPHRASE_RAM ),
        &DCI_SOURCE_AP_PASSPHRASE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_SOURCE_AP_PASSPHRASE_DEFAULT_ATTRIB ),
        DCI_SOURCE_AP_PASSPHRASE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_SOURCE_AP_PASSPHRASE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Source AP Channel
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SOURCE_AP_CHANNEL_RAM ),
        &DCI_SOURCE_AP_CHANNEL_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SOURCE_AP_CHANNEL_DEFAULT_ATTRIB ),
        DCI_SOURCE_AP_CHANNEL_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Source AP Max Conn
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SOURCE_AP_MAX_CONN_RAM ),
        &DCI_SOURCE_AP_MAX_CONN_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SOURCE_AP_MAX_CONN_DEFAULT_ATTRIB ),
        DCI_SOURCE_AP_MAX_CONN_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SNTP timeoffset max in microseconds
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_SNTP_TIME_OFFSET_DEBUG_RAM ),
        &DCI_SNTP_TIME_OFFSET_DEBUG_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_SNTP_TIME_OFFSET_DEBUG_DEFAULT_ATTRIB ),
        DCI_SNTP_TIME_OFFSET_DEBUG_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_SINT64,
    },
    {        // Data Log Spec Version
        DCI_DATA_LOG_SPEC_VERSION_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_DATA_LOG_SPEC_VERSION_RAM ),
        &DCI_DATA_LOG_SPEC_VERSION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_DATA_LOG_SPEC_VERSION_DEFAULT_ATTRIB ),
        DCI_DATA_LOG_SPEC_VERSION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_DATA_LOG_SPEC_VERSION_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Event Log Spec Version
        DCI_EVENT_LOG_SPEC_VERSION_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_EVENT_LOG_SPEC_VERSION_RAM ),
        &DCI_EVENT_LOG_SPEC_VERSION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_EVENT_LOG_SPEC_VERSION_DEFAULT_ATTRIB ),
        DCI_EVENT_LOG_SPEC_VERSION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_EVENT_LOG_SPEC_VERSION_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Audit-Power Log Spec Version
        DCI_AUDIT_POWER_LOG_SPEC_VERSION_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_AUDIT_POWER_LOG_SPEC_VERSION_RAM ),
        &DCI_AUDIT_POWER_LOG_SPEC_VERSION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_POWER_LOG_SPEC_VERSION_DEFAULT_ATTRIB ),
        DCI_AUDIT_POWER_LOG_SPEC_VERSION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_AUDIT_POWER_LOG_SPEC_VERSION_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Audit-Fw-Update Log Spec Version
        DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_RAM ),
        &DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_DEFAULT_ATTRIB ),
        DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Audit-User Log Spec Version
        DCI_AUDIT_USER_LOG_SPEC_VERSION_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_AUDIT_USER_LOG_SPEC_VERSION_RAM ),
        &DCI_AUDIT_USER_LOG_SPEC_VERSION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_USER_LOG_SPEC_VERSION_DEFAULT_ATTRIB ),
        DCI_AUDIT_USER_LOG_SPEC_VERSION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_AUDIT_USER_LOG_SPEC_VERSION_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Audit-Config Log Spec Version
        DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_RAM ),
        &DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_DEFAULT_ATTRIB ),
        DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // NV MAP Version Number
        DCI_NV_MAP_VER_NUM_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_NV_MAP_VER_NUM_RAM ),
        &DCI_NV_MAP_VER_NUM_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_NV_MAP_VER_NUM_DEFAULT_ATTRIB ),
        DCI_NV_MAP_VER_NUM_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT16,
    },
    {        // SecondaryLoadStatus
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_SECONDARY_CONTACT_STATUS_RAM ),
        &DCI_IOT_SECONDARY_CONTACT_STATUS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_SECONDARY_CONTACT_STATUS_DEFAULT_ATTRIB ),
        DCI_IOT_SECONDARY_CONTACT_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_SECONDARY_CONTACT_STATUS_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // NetworkSSID
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_NetworkSSID_RAM ),
        &DCI_IOT_NetworkSSID_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_NetworkSSID_DEFAULT_ATTRIB ),
        DCI_IOT_NetworkSSID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // NetworkPassword
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_NetworkPassword_RAM ),
        &DCI_IOT_NetworkPassword_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_NetworkPassword_DEFAULT_ATTRIB ),
        DCI_IOT_NetworkPassword_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // FirmwareVersion
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_FirmwareVersion_RAM ),
        &DCI_IOT_FirmwareVersion_LENGTH,
        nullptr,
        DCI_IOT_FirmwareVersion_DCID,
        ( 
            0U
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT64,
    },
    {        // MacAddress
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_MacAddress_RAM ),
        &DCI_IOT_MacAddress_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_MacAddress_DEFAULT_ATTRIB ),
        DCI_IOT_MacAddress_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // ResetBreaker
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ResetBreaker_RAM ),
        &DCI_IOT_ResetBreaker_LENGTH,
        nullptr,
        DCI_IOT_ResetBreaker_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK )
        ),
        DCI_IOT_ResetBreaker_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // BuildType
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_BUILD_TYPE_RAM ),
        &DCI_BUILD_TYPE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_BUILD_TYPE_DEFAULT_ATTRIB ),
        DCI_BUILD_TYPE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // NewProgPartNo
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_NEW_PROG_PART_NUMBER_RAM ),
        &DCI_NEW_PROG_PART_NUMBER_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_NEW_PROG_PART_NUMBER_DEFAULT_ATTRIB ),
        DCI_NEW_PROG_PART_NUMBER_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // PrimaryLoadStatus
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_PRIMARY_CONTACT_STATUS_RAM ),
        &DCI_IOT_PRIMARY_CONTACT_STATUS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_PRIMARY_CONTACT_STATUS_DEFAULT_ATTRIB ),
        DCI_IOT_PRIMARY_CONTACT_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PRIMARY_CONTACT_STATUS_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // VoltageL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_VoltageL1_RAM ),
        &DCI_IOT_VoltageL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_VoltageL1_DEFAULT_ATTRIB ),
        DCI_IOT_VoltageL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_VoltageL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // VoltageL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_VoltageL2_RAM ),
        &DCI_IOT_VoltageL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_VoltageL2_DEFAULT_ATTRIB ),
        DCI_IOT_VoltageL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_VoltageL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // CurrentL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_CurrentL1_RAM ),
        &DCI_IOT_CurrentL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_CurrentL1_DEFAULT_ATTRIB ),
        DCI_IOT_CurrentL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_CurrentL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // CurrentL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_CurrentL2_RAM ),
        &DCI_IOT_CurrentL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_CurrentL2_DEFAULT_ATTRIB ),
        DCI_IOT_CurrentL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_CurrentL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // ActivePowerL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ActivePowerL1_RAM ),
        &DCI_IOT_ActivePowerL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ActivePowerL1_DEFAULT_ATTRIB ),
        DCI_IOT_ActivePowerL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ActivePowerL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // ActivePowerL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ActivePowerL2_RAM ),
        &DCI_IOT_ActivePowerL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ActivePowerL2_DEFAULT_ATTRIB ),
        DCI_IOT_ActivePowerL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ActivePowerL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // ReactivePowerL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReactivePowerL1_RAM ),
        &DCI_IOT_ReactivePowerL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReactivePowerL1_DEFAULT_ATTRIB ),
        DCI_IOT_ReactivePowerL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReactivePowerL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // ReactivePowerL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReactivePowerL2_RAM ),
        &DCI_IOT_ReactivePowerL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReactivePowerL2_DEFAULT_ATTRIB ),
        DCI_IOT_ReactivePowerL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReactivePowerL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // ApparentPowerL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ApparentPowerL1_RAM ),
        &DCI_IOT_ApparentPowerL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ApparentPowerL1_DEFAULT_ATTRIB ),
        DCI_IOT_ApparentPowerL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ApparentPowerL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // ApparentPowerL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ApparentPowerL2_RAM ),
        &DCI_IOT_ApparentPowerL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ApparentPowerL2_DEFAULT_ATTRIB ),
        DCI_IOT_ApparentPowerL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ApparentPowerL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // ActiveEnergyL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ActiveEnergyL1_RAM ),
        &DCI_IOT_ActiveEnergyL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ActiveEnergyL1_DEFAULT_ATTRIB ),
        DCI_IOT_ActiveEnergyL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ActiveEnergyL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ActiveEnergyL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ActiveEnergyL2_RAM ),
        &DCI_IOT_ActiveEnergyL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ActiveEnergyL2_DEFAULT_ATTRIB ),
        DCI_IOT_ActiveEnergyL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ActiveEnergyL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ReactiveEnergyL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReactiveEnergyL1_RAM ),
        &DCI_IOT_ReactiveEnergyL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReactiveEnergyL1_DEFAULT_ATTRIB ),
        DCI_IOT_ReactiveEnergyL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReactiveEnergyL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ReactiveEnergyL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReactiveEnergyL2_RAM ),
        &DCI_IOT_ReactiveEnergyL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReactiveEnergyL2_DEFAULT_ATTRIB ),
        DCI_IOT_ReactiveEnergyL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReactiveEnergyL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ApparentEnergyL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ApparentEnergyL1_RAM ),
        &DCI_IOT_ApparentEnergyL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ApparentEnergyL1_DEFAULT_ATTRIB ),
        DCI_IOT_ApparentEnergyL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ApparentEnergyL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ApparentEnergyL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ApparentEnergyL2_RAM ),
        &DCI_IOT_ApparentEnergyL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ApparentEnergyL2_DEFAULT_ATTRIB ),
        DCI_IOT_ApparentEnergyL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ApparentEnergyL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // PowerFactorL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_PowerFactorL1_RAM ),
        &DCI_IOT_PowerFactorL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_PowerFactorL1_DEFAULT_ATTRIB ),
        DCI_IOT_PowerFactorL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PowerFactorL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // PowerFactorL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_PowerFactorL2_RAM ),
        &DCI_IOT_PowerFactorL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_PowerFactorL2_DEFAULT_ATTRIB ),
        DCI_IOT_PowerFactorL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PowerFactorL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // FrequencyL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_FrequencyL1_RAM ),
        &DCI_IOT_FrequencyL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_FrequencyL1_DEFAULT_ATTRIB ),
        DCI_IOT_FrequencyL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_FrequencyL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // FrequencyL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_FrequencyL2_RAM ),
        &DCI_IOT_FrequencyL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_FrequencyL2_DEFAULT_ATTRIB ),
        DCI_IOT_FrequencyL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_FrequencyL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // ReverseActiveEnergyL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReverseActiveEnergyL1_RAM ),
        &DCI_IOT_ReverseActiveEnergyL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReverseActiveEnergyL1_DEFAULT_ATTRIB ),
        DCI_IOT_ReverseActiveEnergyL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReverseActiveEnergyL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ReverseReactiveEnergyL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReverseReactiveEnergyL1_RAM ),
        &DCI_IOT_ReverseReactiveEnergyL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReverseReactiveEnergyL1_DEFAULT_ATTRIB ),
        DCI_IOT_ReverseReactiveEnergyL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReverseReactiveEnergyL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ReverseApparentEnergyL1
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReverseApparentEnergyL1_RAM ),
        &DCI_IOT_ReverseApparentEnergyL1_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReverseApparentEnergyL1_DEFAULT_ATTRIB ),
        DCI_IOT_ReverseApparentEnergyL1_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReverseApparentEnergyL1_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ReverseActiveEnergyL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReverseActiveEnergyL2_RAM ),
        &DCI_IOT_ReverseActiveEnergyL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReverseActiveEnergyL2_DEFAULT_ATTRIB ),
        DCI_IOT_ReverseActiveEnergyL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReverseActiveEnergyL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ReverseReactiveEnergyL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReverseReactiveEnergyL2_RAM ),
        &DCI_IOT_ReverseReactiveEnergyL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReverseReactiveEnergyL2_DEFAULT_ATTRIB ),
        DCI_IOT_ReverseReactiveEnergyL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReverseReactiveEnergyL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // ReverseApparentEnergyL2
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_ReverseApparentEnergyL2_RAM ),
        &DCI_IOT_ReverseApparentEnergyL2_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_ReverseApparentEnergyL2_DEFAULT_ATTRIB ),
        DCI_IOT_ReverseApparentEnergyL2_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_ReverseApparentEnergyL2_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_DFLOAT,
    },
    {        // Unicast Primary UDP key
        DCI_IOT_UNICAST_PRIMARY_UDP_KEY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_UNICAST_PRIMARY_UDP_KEY_RAM ),
        &DCI_IOT_UNICAST_PRIMARY_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_UNICAST_PRIMARY_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_UNICAST_PRIMARY_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_UNICAST_PRIMARY_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Get Unicast UDP key
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_GET_UNICAST_UDP_KEY_RAM ),
        &DCI_IOT_GET_UNICAST_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_GET_UNICAST_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_GET_UNICAST_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_GET_UNICAST_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Broadcast Primary UDP key
        DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_RAM ),
        &DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Get Broadcast UDP key
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_GET_BROADCAST_UDP_KEY_RAM ),
        &DCI_IOT_GET_BROADCAST_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_GET_BROADCAST_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_GET_BROADCAST_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_GET_BROADCAST_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Unicast secondary UDP key
        DCI_IOT_UNICAST_SECONDARY_UDP_KEY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_UNICAST_SECONDARY_UDP_KEY_RAM ),
        &DCI_IOT_UNICAST_SECONDARY_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_UNICAST_SECONDARY_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_UNICAST_SECONDARY_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_UNICAST_SECONDARY_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Get secondary Unicast UDP key
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_RAM ),
        &DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Broadcast secondary UDP key
        DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_RAM ),
        &DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // Get Broadcast secondary UDP key
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_RAM ),
        &DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // ErrorLogData
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_ErrorLogData_RAM ),
        &DCI_IOT_ErrorLogData_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( DCI_IOT_ErrorLogData_DEFAULT_ATTRIB ),
        DCI_IOT_ErrorLogData_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // PathStatus
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_PATH_STATUS_RAM ),
        &DCI_IOT_PATH_STATUS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_PATH_STATUS_DEFAULT_ATTRIB ),
        DCI_IOT_PATH_STATUS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PATH_STATUS_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // PrimarySwitchState
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_PRIMARY_STATE_RAM ),
        &DCI_IOT_PRIMARY_STATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_PRIMARY_STATE_DEFAULT_ATTRIB ),
        DCI_IOT_PRIMARY_STATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_PRIMARY_STATE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // SecondarySwitchState
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_SECONDARY_STATE_RAM ),
        &DCI_IOT_SECONDARY_STATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_SECONDARY_STATE_DEFAULT_ATTRIB ),
        DCI_IOT_SECONDARY_STATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_SECONDARY_STATE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Delete Primary Unicast UDP Key
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_RAM ),
        &DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Delete Primary Broadcast UDP Key
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_RAM ),
        &DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Delete Secondary Unicast UDP Key
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_RAM ),
        &DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // Delete Secondary Broadcast UDP Key
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_RAM ),
        &DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_DEFAULT_ATTRIB ),
        DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
    {        // PercentageLoad
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_Load_Percentage_RAM ),
        &DCI_IOT_Load_Percentage_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_Load_Percentage_DEFAULT_ATTRIB ),
        DCI_IOT_Load_Percentage_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_Load_Percentage_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // IOT Device Provisioning URL
        DCI_IOT_DPS_ENDPOINT_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_DPS_ENDPOINT_RAM ),
        &DCI_IOT_DPS_ENDPOINT_LENGTH,
        nullptr,
        DCI_IOT_DPS_ENDPOINT_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK )
        ),
        DCI_IOT_DPS_ENDPOINT_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // IOT Device Provisioning ID Scope
        DCI_IOT_DPS_ID_SCOPE_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_DPS_ID_SCOPE_RAM ),
        &DCI_IOT_DPS_ID_SCOPE_LENGTH,
        nullptr,
        DCI_IOT_DPS_ID_SCOPE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK )
        ),
        DCI_IOT_DPS_ID_SCOPE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // IOT Device Provisioning Symmetric Key
        DCI_IOT_DPS_SYMM_KEY_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_DPS_SYMM_KEY_RAM ),
        &DCI_IOT_DPS_SYMM_KEY_LENGTH,
        nullptr,
        DCI_IOT_DPS_SYMM_KEY_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK )
        ),
        DCI_IOT_DPS_SYMM_KEY_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // IOT Device Provisioning Device Registration ID
        DCI_IOT_DPS_DEVICE_REG_ID_NVADD,        //Not Default
        reinterpret_cast<DCI_RAM_DATA_TD *>( DCI_IOT_DPS_DEVICE_REG_ID_RAM ),
        &DCI_IOT_DPS_DEVICE_REG_ID_LENGTH,
        nullptr,
        DCI_IOT_DPS_DEVICE_REG_ID_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK )
        ),
        DCI_IOT_DPS_DEVICE_REG_ID_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_STRING8,
    },
    {        // IdentifyMe
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_IDENTIFY_ME_RAM ),
        &DCI_IOT_IDENTIFY_ME_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_IDENTIFY_ME_DEFAULT_ATTRIB ),
        DCI_IOT_IDENTIFY_ME_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_IDENTIFY_ME_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // WiFi Rssi Signal Strength
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_RAM ),
        &DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_DEFAULT_ATTRIB ),
        DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT8,
    },
    {        // Temperature
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_TEMPERATURE_RAM ),
        &DCI_IOT_TEMPERATURE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_TEMPERATURE_DEFAULT_ATTRIB ),
        DCI_IOT_TEMPERATURE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        0xFFFF,    //No Callback index Allocated.
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_FLOAT,
    },
    {        // Power Up State
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_POWER_UP_STATE_RAM ),
        &DCI_IOT_POWER_UP_STATE_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_POWER_UP_STATE_DEFAULT_ATTRIB ),
        DCI_IOT_POWER_UP_STATE_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_POWER_UP_STATE_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_UINT32,
    },
    {        // SBLCP Enable Disable
        DCI_DATA_NO_NV_MEM_DATA,
        reinterpret_cast<DCI_RAM_DATA_TD *>( &DCI_IOT_SBLCP_EN_DIS_RAM ),
        &DCI_IOT_SBLCP_EN_DIS_LENGTH,
        reinterpret_cast<DCI_DEF_DATA_TD const*>( &DCI_IOT_SBLCP_EN_DIS_DEFAULT_ATTRIB ),
        DCI_IOT_SBLCP_EN_DIS_DCID,
        ( 
             ( DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK ) | 
             ( DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK )
        ),
        ( 
             ( DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK ) | 
             ( DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK )
        ),
        DCI_IOT_SBLCP_EN_DIS_CBACK_INDEX,        //Not Default
        0xFFFF, //No range block index Allocated.
        DCI_DTYPE_BOOL,
    },
};
