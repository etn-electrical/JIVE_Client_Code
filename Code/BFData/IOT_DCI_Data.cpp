/**
*****************************************************************************************
*  @file
*  @details See header file for module overview.
*  @copyright 2018 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/

#include "Includes.h"
#include "IOT_DCI_Data.h"

//*******************************************************
//******     IOT Misc Definition 
//*******************************************************

static const uint16_t IOT_DCI_DATA_TOTAL_CHANNELS = 61U;
static const uint16_t IOT_DCI_DATA_TOTAL_GROUPS = 4;


//*******************************************************
//******     IOT Channel List 
//*******************************************************

static const uint32_t DCI_IOT_SECONDARY_CONTACT_STATUS_IOT_CHANNEL_ID = 112078U;
static const uint32_t DCI_IOT_NetworkSSID_IOT_CHANNEL_ID = 112083U;
static const uint32_t DCI_IOT_NetworkPassword_IOT_CHANNEL_ID = 112084U;
static const uint32_t DCI_IOT_FirmwareVersion_IOT_CHANNEL_ID = 112086U;
static const uint32_t DCI_IOT_MacAddress_IOT_CHANNEL_ID = 112094U;
static const uint32_t DCI_IOT_ResetBreaker_IOT_CHANNEL_ID = 112105U;
static const uint32_t DCI_PRODUCT_NAME_IOT_CHANNEL_ID = 10014222U;
static const uint32_t DCI_NEW_PROG_PART_NUMBER_IOT_CHANNEL_ID = 10034175U;
static const uint32_t DCI_BUILD_TYPE_IOT_CHANNEL_ID = 10034176U;
static const uint32_t DCI_IOT_PRIMARY_CONTACT_STATUS_IOT_CHANNEL_ID = 10083391U;
static const uint32_t DCI_IOT_VoltageL1_IOT_CHANNEL_ID = 10083394U;
static const uint32_t DCI_IOT_VoltageL2_IOT_CHANNEL_ID = 10083395U;
static const uint32_t DCI_IOT_CurrentL1_IOT_CHANNEL_ID = 10083396U;
static const uint32_t DCI_IOT_CurrentL2_IOT_CHANNEL_ID = 10083397U;
static const uint32_t DCI_IOT_ActivePowerL1_IOT_CHANNEL_ID = 10134540U;
static const uint32_t DCI_IOT_ActivePowerL2_IOT_CHANNEL_ID = 10134543U;
static const uint32_t DCI_IOT_ReactivePowerL1_IOT_CHANNEL_ID = 10134541U;
static const uint32_t DCI_IOT_ReactivePowerL2_IOT_CHANNEL_ID = 10134544U;
static const uint32_t DCI_IOT_ApparentPowerL1_IOT_CHANNEL_ID = 10134542U;
static const uint32_t DCI_IOT_ApparentPowerL2_IOT_CHANNEL_ID = 10134545U;
static const uint32_t DCI_IOT_PowerFactorL1_IOT_CHANNEL_ID = 10083402U;
static const uint32_t DCI_IOT_PowerFactorL2_IOT_CHANNEL_ID = 10083403U;
static const uint32_t DCI_IOT_FrequencyL1_IOT_CHANNEL_ID = 10134552U;
static const uint32_t DCI_IOT_FrequencyL2_IOT_CHANNEL_ID = 10134553U;
static const uint32_t DCI_IOT_ActiveEnergyL1_IOT_CHANNEL_ID = 10134546U;
static const uint32_t DCI_IOT_ActiveEnergyL2_IOT_CHANNEL_ID = 10134549U;
static const uint32_t DCI_IOT_ReactiveEnergyL1_IOT_CHANNEL_ID = 10134547U;
static const uint32_t DCI_IOT_ReactiveEnergyL2_IOT_CHANNEL_ID = 10134550U;
static const uint32_t DCI_IOT_ApparentEnergyL1_IOT_CHANNEL_ID = 10134548U;
static const uint32_t DCI_IOT_ApparentEnergyL2_IOT_CHANNEL_ID = 10134551U;
static const uint32_t DCI_IOT_ReverseActiveEnergyL1_IOT_CHANNEL_ID = 10153974U;
static const uint32_t DCI_IOT_ReverseReactiveEnergyL1_IOT_CHANNEL_ID = 10153975U;
static const uint32_t DCI_IOT_ReverseApparentEnergyL1_IOT_CHANNEL_ID = 10153976U;
static const uint32_t DCI_IOT_ReverseActiveEnergyL2_IOT_CHANNEL_ID = 10153977U;
static const uint32_t DCI_IOT_ReverseReactiveEnergyL2_IOT_CHANNEL_ID = 10153978U;
static const uint32_t DCI_IOT_ReverseApparentEnergyL2_IOT_CHANNEL_ID = 10153979U;
static const uint32_t DCI_IOT_UNICAST_PRIMARY_UDP_KEY_IOT_CHANNEL_ID = 10192897U;
static const uint32_t DCI_IOT_GET_UNICAST_UDP_KEY_IOT_CHANNEL_ID = 10186911U;
static const uint32_t DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_IOT_CHANNEL_ID = 10192899U;
static const uint32_t DCI_IOT_GET_BROADCAST_UDP_KEY_IOT_CHANNEL_ID = 10186910U;
static const uint32_t DCI_IOT_UNICAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID = 10192898U;
static const uint32_t DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID = 10192326U;
static const uint32_t DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID = 10192900U;
static const uint32_t DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID = 10192328U;
static const uint32_t DCI_IOT_ErrorLogData_IOT_CHANNEL_ID = 10083415U;
static const uint32_t DCI_IOT_PATH_STATUS_IOT_CHANNEL_ID = 10228020U;
static const uint32_t DCI_IOT_PRIMARY_STATE_IOT_CHANNEL_ID = 10228021U;
static const uint32_t DCI_IOT_SECONDARY_STATE_IOT_CHANNEL_ID = 10228022U;
static const uint32_t DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_IOT_CHANNEL_ID = 10273357U;
static const uint32_t DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_IOT_CHANNEL_ID = 10273358U;
static const uint32_t DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_IOT_CHANNEL_ID = 10273809U;
static const uint32_t DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_IOT_CHANNEL_ID = 10273810U;
static const uint32_t DCI_IOT_Load_Percentage_IOT_CHANNEL_ID = 10288155U;
static const uint32_t DCI_IOT_DPS_ID_SCOPE_IOT_CHANNEL_ID = 10186063U;
static const uint32_t DCI_IOT_DPS_SYMM_KEY_IOT_CHANNEL_ID = 10186064U;
static const uint32_t DCI_IOT_DPS_DEVICE_REG_ID_IOT_CHANNEL_ID = 10190018U;
static const uint32_t DCI_IOT_IDENTIFY_ME_IOT_CHANNEL_ID = 10289358U;
static const uint32_t DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_IOT_CHANNEL_ID = 10290885U;
static const uint32_t DCI_IOT_TEMPERATURE_IOT_CHANNEL_ID = 10302277U;
static const uint32_t DCI_IOT_POWER_UP_STATE_IOT_CHANNEL_ID = 112079U;
static const uint32_t DCI_IOT_SBLCP_EN_DIS_IOT_CHANNEL_ID = 10306405U;


static const char_t DCI_IOT_SECONDARY_CONTACT_STATUS_IOT_CHANNEL_ID_STR[] = "112078";
static const char_t DCI_IOT_NetworkSSID_IOT_CHANNEL_ID_STR[] = "112083";
static const char_t DCI_IOT_NetworkPassword_IOT_CHANNEL_ID_STR[] = "112084";
static const char_t DCI_IOT_FirmwareVersion_IOT_CHANNEL_ID_STR[] = "112086";
static const char_t DCI_IOT_MacAddress_IOT_CHANNEL_ID_STR[] = "112094";
static const char_t DCI_IOT_ResetBreaker_IOT_CHANNEL_ID_STR[] = "112105";
static const char_t DCI_PRODUCT_NAME_IOT_CHANNEL_ID_STR[] = "10014222";
static const char_t DCI_NEW_PROG_PART_NUMBER_IOT_CHANNEL_ID_STR[] = "10034175";
static const char_t DCI_BUILD_TYPE_IOT_CHANNEL_ID_STR[] = "10034176";
static const char_t DCI_IOT_PRIMARY_CONTACT_STATUS_IOT_CHANNEL_ID_STR[] = "10083391";
static const char_t DCI_IOT_VoltageL1_IOT_CHANNEL_ID_STR[] = "10083394";
static const char_t DCI_IOT_VoltageL2_IOT_CHANNEL_ID_STR[] = "10083395";
static const char_t DCI_IOT_CurrentL1_IOT_CHANNEL_ID_STR[] = "10083396";
static const char_t DCI_IOT_CurrentL2_IOT_CHANNEL_ID_STR[] = "10083397";
static const char_t DCI_IOT_ActivePowerL1_IOT_CHANNEL_ID_STR[] = "10134540";
static const char_t DCI_IOT_ActivePowerL2_IOT_CHANNEL_ID_STR[] = "10134543";
static const char_t DCI_IOT_ReactivePowerL1_IOT_CHANNEL_ID_STR[] = "10134541";
static const char_t DCI_IOT_ReactivePowerL2_IOT_CHANNEL_ID_STR[] = "10134544";
static const char_t DCI_IOT_ApparentPowerL1_IOT_CHANNEL_ID_STR[] = "10134542";
static const char_t DCI_IOT_ApparentPowerL2_IOT_CHANNEL_ID_STR[] = "10134545";
static const char_t DCI_IOT_PowerFactorL1_IOT_CHANNEL_ID_STR[] = "10083402";
static const char_t DCI_IOT_PowerFactorL2_IOT_CHANNEL_ID_STR[] = "10083403";
static const char_t DCI_IOT_FrequencyL1_IOT_CHANNEL_ID_STR[] = "10134552";
static const char_t DCI_IOT_FrequencyL2_IOT_CHANNEL_ID_STR[] = "10134553";
static const char_t DCI_IOT_ActiveEnergyL1_IOT_CHANNEL_ID_STR[] = "10134546";
static const char_t DCI_IOT_ActiveEnergyL2_IOT_CHANNEL_ID_STR[] = "10134549";
static const char_t DCI_IOT_ReactiveEnergyL1_IOT_CHANNEL_ID_STR[] = "10134547";
static const char_t DCI_IOT_ReactiveEnergyL2_IOT_CHANNEL_ID_STR[] = "10134550";
static const char_t DCI_IOT_ApparentEnergyL1_IOT_CHANNEL_ID_STR[] = "10134548";
static const char_t DCI_IOT_ApparentEnergyL2_IOT_CHANNEL_ID_STR[] = "10134551";
static const char_t DCI_IOT_ReverseActiveEnergyL1_IOT_CHANNEL_ID_STR[] = "10153974";
static const char_t DCI_IOT_ReverseReactiveEnergyL1_IOT_CHANNEL_ID_STR[] = "10153975";
static const char_t DCI_IOT_ReverseApparentEnergyL1_IOT_CHANNEL_ID_STR[] = "10153976";
static const char_t DCI_IOT_ReverseActiveEnergyL2_IOT_CHANNEL_ID_STR[] = "10153977";
static const char_t DCI_IOT_ReverseReactiveEnergyL2_IOT_CHANNEL_ID_STR[] = "10153978";
static const char_t DCI_IOT_ReverseApparentEnergyL2_IOT_CHANNEL_ID_STR[] = "10153979";
static const char_t DCI_IOT_UNICAST_PRIMARY_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10192897";
static const char_t DCI_IOT_GET_UNICAST_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10186911";
static const char_t DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_IOT_CHANNEL_ID_STR[] = "10192899";
static const char_t DCI_IOT_GET_BROADCAST_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10186910";
static const char_t DCI_IOT_UNICAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10192898";
static const char_t DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10192326";
static const char_t DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10192900";
static const char_t DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10192328";
static const char_t DCI_IOT_ErrorLogData_IOT_CHANNEL_ID_STR[] = "10083415";
static const char_t DCI_IOT_PATH_STATUS_IOT_CHANNEL_ID_STR[] = "10228020";
static const char_t DCI_IOT_PRIMARY_STATE_IOT_CHANNEL_ID_STR[] = "10228021";
static const char_t DCI_IOT_SECONDARY_STATE_IOT_CHANNEL_ID_STR[] = "10228022";
static const char_t DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10273357";
static const char_t DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10273358";
static const char_t DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10273809";
static const char_t DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_IOT_CHANNEL_ID_STR[] = "10273810";
static const char_t DCI_IOT_Load_Percentage_IOT_CHANNEL_ID_STR[] = "10288155";
static const char_t DCI_IOT_DPS_ID_SCOPE_IOT_CHANNEL_ID_STR[] = "10186063";
static const char_t DCI_IOT_DPS_SYMM_KEY_IOT_CHANNEL_ID_STR[] = "10186064";
static const char_t DCI_IOT_DPS_DEVICE_REG_ID_IOT_CHANNEL_ID_STR[] = "10190018";
static const char_t DCI_IOT_IDENTIFY_ME_IOT_CHANNEL_ID_STR[] = "10289358";
static const char_t DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_IOT_CHANNEL_ID_STR[] = "10290885";
static const char_t DCI_IOT_TEMPERATURE_IOT_CHANNEL_ID_STR[] = "10302277";
static const char_t DCI_IOT_POWER_UP_STATE_IOT_CHANNEL_ID_STR[] = "112079";
static const char_t DCI_IOT_SBLCP_EN_DIS_IOT_CHANNEL_ID_STR[] = "10306405";

//*******************************************************
//******     IOT Channel Array Offset List 
//*******************************************************

static const uint16_t DCI_IOT_SECONDARY_CONTACT_STATUS_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_NetworkSSID_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_NetworkPassword_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_FirmwareVersion_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_MacAddress_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ResetBreaker_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_PRODUCT_NAME_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_NEW_PROG_PART_NUMBER_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_BUILD_TYPE_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_PRIMARY_CONTACT_STATUS_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_VoltageL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_VoltageL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_CurrentL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_CurrentL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ActivePowerL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ActivePowerL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReactivePowerL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReactivePowerL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ApparentPowerL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ApparentPowerL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_PowerFactorL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_PowerFactorL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_FrequencyL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_FrequencyL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ActiveEnergyL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ActiveEnergyL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReactiveEnergyL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReactiveEnergyL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ApparentEnergyL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ApparentEnergyL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReverseActiveEnergyL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReverseReactiveEnergyL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReverseApparentEnergyL1_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReverseActiveEnergyL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReverseReactiveEnergyL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ReverseApparentEnergyL2_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_UNICAST_PRIMARY_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_GET_UNICAST_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_GET_BROADCAST_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_UNICAST_SECONDARY_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_ErrorLogData_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_PATH_STATUS_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_PRIMARY_STATE_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_SECONDARY_STATE_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_Load_Percentage_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_DPS_ID_SCOPE_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_DPS_SYMM_KEY_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_DPS_DEVICE_REG_ID_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_IDENTIFY_ME_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_TEMPERATURE_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_POWER_UP_STATE_IOT_ARRAY_OFFSET = 0U;
static const uint16_t DCI_IOT_SBLCP_EN_DIS_IOT_ARRAY_OFFSET = 0U;

//*******************************************************
//******     The IOT to DCID Cross Reference table.
//*******************************************************

static const dci_iot_to_dcid_lkup_st_t iot_dci_data_iot_to_dcid[IOT_DCI_DATA_TOTAL_CHANNELS] =
{
    {
        // SecondaryLoadStatus
        DCI_IOT_SECONDARY_CONTACT_STATUS_IOT_CHANNEL_ID,
        DCI_IOT_SECONDARY_CONTACT_STATUS_IOT_CHANNEL_ID_STR,
        DCI_IOT_SECONDARY_CONTACT_STATUS_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_SECONDARY_CONTACT_STATUS_IOT_ARRAY_OFFSET
    },
    {
        // NetworkSSID
        DCI_IOT_NetworkSSID_IOT_CHANNEL_ID,
        DCI_IOT_NetworkSSID_IOT_CHANNEL_ID_STR,
        DCI_IOT_NetworkSSID_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_NetworkSSID_IOT_ARRAY_OFFSET
    },
    {
        // NetworkPassword
        DCI_IOT_NetworkPassword_IOT_CHANNEL_ID,
        DCI_IOT_NetworkPassword_IOT_CHANNEL_ID_STR,
        DCI_IOT_NetworkPassword_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_NetworkPassword_IOT_ARRAY_OFFSET
    },
    {
        // FirmwareVersion
        DCI_IOT_FirmwareVersion_IOT_CHANNEL_ID,
        DCI_IOT_FirmwareVersion_IOT_CHANNEL_ID_STR,
        DCI_IOT_FirmwareVersion_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_FirmwareVersion_IOT_ARRAY_OFFSET
    },
    {
        // MacAddress
        DCI_IOT_MacAddress_IOT_CHANNEL_ID,
        DCI_IOT_MacAddress_IOT_CHANNEL_ID_STR,
        DCI_IOT_MacAddress_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_MacAddress_IOT_ARRAY_OFFSET
    },
    {
        // ResetBreaker
        DCI_IOT_ResetBreaker_IOT_CHANNEL_ID,
        DCI_IOT_ResetBreaker_IOT_CHANNEL_ID_STR,
        DCI_IOT_ResetBreaker_DCID,
        IOT_DCI::CADENCE_GROUP_NONE,
        DCI_IOT_ResetBreaker_IOT_ARRAY_OFFSET
    },
    {
        // Product Name 
        DCI_PRODUCT_NAME_IOT_CHANNEL_ID,
        DCI_PRODUCT_NAME_IOT_CHANNEL_ID_STR,
        DCI_PRODUCT_NAME_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_PRODUCT_NAME_IOT_ARRAY_OFFSET
    },
    {
        // NewProgPartNo
        DCI_NEW_PROG_PART_NUMBER_IOT_CHANNEL_ID,
        DCI_NEW_PROG_PART_NUMBER_IOT_CHANNEL_ID_STR,
        DCI_NEW_PROG_PART_NUMBER_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_NEW_PROG_PART_NUMBER_IOT_ARRAY_OFFSET
    },
    {
        // BuildType
        DCI_BUILD_TYPE_IOT_CHANNEL_ID,
        DCI_BUILD_TYPE_IOT_CHANNEL_ID_STR,
        DCI_BUILD_TYPE_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_BUILD_TYPE_IOT_ARRAY_OFFSET
    },
    {
        // PrimaryLoadStatus
        DCI_IOT_PRIMARY_CONTACT_STATUS_IOT_CHANNEL_ID,
        DCI_IOT_PRIMARY_CONTACT_STATUS_IOT_CHANNEL_ID_STR,
        DCI_IOT_PRIMARY_CONTACT_STATUS_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_PRIMARY_CONTACT_STATUS_IOT_ARRAY_OFFSET
    },
    {
        // VoltageL1
        DCI_IOT_VoltageL1_IOT_CHANNEL_ID,
        DCI_IOT_VoltageL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_VoltageL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_VoltageL1_IOT_ARRAY_OFFSET
    },
    {
        // VoltageL2
        DCI_IOT_VoltageL2_IOT_CHANNEL_ID,
        DCI_IOT_VoltageL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_VoltageL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_VoltageL2_IOT_ARRAY_OFFSET
    },
    {
        // CurrentL1
        DCI_IOT_CurrentL1_IOT_CHANNEL_ID,
        DCI_IOT_CurrentL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_CurrentL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_CurrentL1_IOT_ARRAY_OFFSET
    },
    {
        // CurrentL2
        DCI_IOT_CurrentL2_IOT_CHANNEL_ID,
        DCI_IOT_CurrentL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_CurrentL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_CurrentL2_IOT_ARRAY_OFFSET
    },
    {
        // ActivePowerL1
        DCI_IOT_ActivePowerL1_IOT_CHANNEL_ID,
        DCI_IOT_ActivePowerL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ActivePowerL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ActivePowerL1_IOT_ARRAY_OFFSET
    },
    {
        // ActivePowerL2
        DCI_IOT_ActivePowerL2_IOT_CHANNEL_ID,
        DCI_IOT_ActivePowerL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ActivePowerL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ActivePowerL2_IOT_ARRAY_OFFSET
    },
    {
        // ReactivePowerL1
        DCI_IOT_ReactivePowerL1_IOT_CHANNEL_ID,
        DCI_IOT_ReactivePowerL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReactivePowerL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReactivePowerL1_IOT_ARRAY_OFFSET
    },
    {
        // ReactivePowerL2
        DCI_IOT_ReactivePowerL2_IOT_CHANNEL_ID,
        DCI_IOT_ReactivePowerL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReactivePowerL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReactivePowerL2_IOT_ARRAY_OFFSET
    },
    {
        // ApparentPowerL1
        DCI_IOT_ApparentPowerL1_IOT_CHANNEL_ID,
        DCI_IOT_ApparentPowerL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ApparentPowerL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ApparentPowerL1_IOT_ARRAY_OFFSET
    },
    {
        // ApparentPowerL2
        DCI_IOT_ApparentPowerL2_IOT_CHANNEL_ID,
        DCI_IOT_ApparentPowerL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ApparentPowerL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ApparentPowerL2_IOT_ARRAY_OFFSET
    },
    {
        // PowerFactorL1
        DCI_IOT_PowerFactorL1_IOT_CHANNEL_ID,
        DCI_IOT_PowerFactorL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_PowerFactorL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_PowerFactorL1_IOT_ARRAY_OFFSET
    },
    {
        // PowerFactorL2
        DCI_IOT_PowerFactorL2_IOT_CHANNEL_ID,
        DCI_IOT_PowerFactorL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_PowerFactorL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_PowerFactorL2_IOT_ARRAY_OFFSET
    },
    {
        // FrequencyL1
        DCI_IOT_FrequencyL1_IOT_CHANNEL_ID,
        DCI_IOT_FrequencyL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_FrequencyL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_FrequencyL1_IOT_ARRAY_OFFSET
    },
    {
        // FrequencyL2
        DCI_IOT_FrequencyL2_IOT_CHANNEL_ID,
        DCI_IOT_FrequencyL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_FrequencyL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_FrequencyL2_IOT_ARRAY_OFFSET
    },
    {
        // ActiveEnergyL1
        DCI_IOT_ActiveEnergyL1_IOT_CHANNEL_ID,
        DCI_IOT_ActiveEnergyL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ActiveEnergyL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ActiveEnergyL1_IOT_ARRAY_OFFSET
    },
    {
        // ActiveEnergyL2
        DCI_IOT_ActiveEnergyL2_IOT_CHANNEL_ID,
        DCI_IOT_ActiveEnergyL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ActiveEnergyL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ActiveEnergyL2_IOT_ARRAY_OFFSET
    },
    {
        // ReactiveEnergyL1
        DCI_IOT_ReactiveEnergyL1_IOT_CHANNEL_ID,
        DCI_IOT_ReactiveEnergyL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReactiveEnergyL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReactiveEnergyL1_IOT_ARRAY_OFFSET
    },
    {
        // ReactiveEnergyL2
        DCI_IOT_ReactiveEnergyL2_IOT_CHANNEL_ID,
        DCI_IOT_ReactiveEnergyL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReactiveEnergyL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReactiveEnergyL2_IOT_ARRAY_OFFSET
    },
    {
        // ApparentEnergyL1
        DCI_IOT_ApparentEnergyL1_IOT_CHANNEL_ID,
        DCI_IOT_ApparentEnergyL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ApparentEnergyL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ApparentEnergyL1_IOT_ARRAY_OFFSET
    },
    {
        // ApparentEnergyL2
        DCI_IOT_ApparentEnergyL2_IOT_CHANNEL_ID,
        DCI_IOT_ApparentEnergyL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ApparentEnergyL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ApparentEnergyL2_IOT_ARRAY_OFFSET
    },
    {
        // ReverseActiveEnergyL1
        DCI_IOT_ReverseActiveEnergyL1_IOT_CHANNEL_ID,
        DCI_IOT_ReverseActiveEnergyL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReverseActiveEnergyL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReverseActiveEnergyL1_IOT_ARRAY_OFFSET
    },
    {
        // ReverseReactiveEnergyL1
        DCI_IOT_ReverseReactiveEnergyL1_IOT_CHANNEL_ID,
        DCI_IOT_ReverseReactiveEnergyL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReverseReactiveEnergyL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReverseReactiveEnergyL1_IOT_ARRAY_OFFSET
    },
    {
        // ReverseApparentEnergyL1
        DCI_IOT_ReverseApparentEnergyL1_IOT_CHANNEL_ID,
        DCI_IOT_ReverseApparentEnergyL1_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReverseApparentEnergyL1_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReverseApparentEnergyL1_IOT_ARRAY_OFFSET
    },
    {
        // ReverseActiveEnergyL2
        DCI_IOT_ReverseActiveEnergyL2_IOT_CHANNEL_ID,
        DCI_IOT_ReverseActiveEnergyL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReverseActiveEnergyL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReverseActiveEnergyL2_IOT_ARRAY_OFFSET
    },
    {
        // ReverseReactiveEnergyL2
        DCI_IOT_ReverseReactiveEnergyL2_IOT_CHANNEL_ID,
        DCI_IOT_ReverseReactiveEnergyL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReverseReactiveEnergyL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReverseReactiveEnergyL2_IOT_ARRAY_OFFSET
    },
    {
        // ReverseApparentEnergyL2
        DCI_IOT_ReverseApparentEnergyL2_IOT_CHANNEL_ID,
        DCI_IOT_ReverseApparentEnergyL2_IOT_CHANNEL_ID_STR,
        DCI_IOT_ReverseApparentEnergyL2_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ReverseApparentEnergyL2_IOT_ARRAY_OFFSET
    },
    {
        // Unicast Primary UDP key
        DCI_IOT_UNICAST_PRIMARY_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_UNICAST_PRIMARY_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_UNICAST_PRIMARY_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_UNICAST_PRIMARY_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Get Unicast UDP key
        DCI_IOT_GET_UNICAST_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_GET_UNICAST_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_GET_UNICAST_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_GET_UNICAST_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Broadcast Primary UDP key
        DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_IOT_CHANNEL_ID,
        DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Get Broadcast UDP key
        DCI_IOT_GET_BROADCAST_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_GET_BROADCAST_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_GET_BROADCAST_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_GET_BROADCAST_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Unicast Secondary UDP key
        DCI_IOT_UNICAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_UNICAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_UNICAST_SECONDARY_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_UNICAST_SECONDARY_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Get Unicast Secondary UDP key
        DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Broadcast Secondary UDP key
        DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Get Broadcast Secondary UDP key
        DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // ErrorLogData
        DCI_IOT_ErrorLogData_IOT_CHANNEL_ID,
        DCI_IOT_ErrorLogData_IOT_CHANNEL_ID_STR,
        DCI_IOT_ErrorLogData_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_ErrorLogData_IOT_ARRAY_OFFSET
    },
    {
        // PathStatus
        DCI_IOT_PATH_STATUS_IOT_CHANNEL_ID,
        DCI_IOT_PATH_STATUS_IOT_CHANNEL_ID_STR,
        DCI_IOT_PATH_STATUS_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_PATH_STATUS_IOT_ARRAY_OFFSET
    },
    {
        // PrimarySwitchState
        DCI_IOT_PRIMARY_STATE_IOT_CHANNEL_ID,
        DCI_IOT_PRIMARY_STATE_IOT_CHANNEL_ID_STR,
        DCI_IOT_PRIMARY_STATE_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_PRIMARY_STATE_IOT_ARRAY_OFFSET
    },
    {
        // SecondarySwitchState
        DCI_IOT_SECONDARY_STATE_IOT_CHANNEL_ID,
        DCI_IOT_SECONDARY_STATE_IOT_CHANNEL_ID_STR,
        DCI_IOT_SECONDARY_STATE_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_SECONDARY_STATE_IOT_ARRAY_OFFSET
    },
    {
        // Delete Primary Unicast UDP Key
        DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_DELETE_PRIMARY_UNICAST_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Delete Primary Broadcast UDP Key
        DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_DELETE_PRIMARY_BROADCAST_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Delete Secondary Unicast UDP Key
        DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_DELETE_SECONDARY_UNICAST_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // Delete Secondary Broadcast UDP Key
        DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_IOT_CHANNEL_ID,
        DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_DELETE_SECONDARY_BROADCAST_UDP_KEY_IOT_ARRAY_OFFSET
    },
    {
        // PercentageLoad
        DCI_IOT_Load_Percentage_IOT_CHANNEL_ID,
        DCI_IOT_Load_Percentage_IOT_CHANNEL_ID_STR,
        DCI_IOT_Load_Percentage_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_Load_Percentage_IOT_ARRAY_OFFSET
    },
    {
        // IOT Device Provisioning ID Scope
        DCI_IOT_DPS_ID_SCOPE_IOT_CHANNEL_ID,
        DCI_IOT_DPS_ID_SCOPE_IOT_CHANNEL_ID_STR,
        DCI_IOT_DPS_ID_SCOPE_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_DPS_ID_SCOPE_IOT_ARRAY_OFFSET
    },
    {
        // IOT Device Provisioning Symmetric Key
        DCI_IOT_DPS_SYMM_KEY_IOT_CHANNEL_ID,
        DCI_IOT_DPS_SYMM_KEY_IOT_CHANNEL_ID_STR,
        DCI_IOT_DPS_SYMM_KEY_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_DPS_SYMM_KEY_IOT_ARRAY_OFFSET
    },
    {
        // IOT Device Provisioning Device Registration ID
        DCI_IOT_DPS_DEVICE_REG_ID_IOT_CHANNEL_ID,
        DCI_IOT_DPS_DEVICE_REG_ID_IOT_CHANNEL_ID_STR,
        DCI_IOT_DPS_DEVICE_REG_ID_DCID,
        IOT_DCI::CADENCE_GROUP0,
        DCI_IOT_DPS_DEVICE_REG_ID_IOT_ARRAY_OFFSET
    },
    {
        // Identify Me
        DCI_IOT_IDENTIFY_ME_IOT_CHANNEL_ID,
        DCI_IOT_IDENTIFY_ME_IOT_CHANNEL_ID_STR,
        DCI_IOT_IDENTIFY_ME_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_IDENTIFY_ME_IOT_ARRAY_OFFSET
    },
    {
        // WiFi Rssi Signal Strength
        DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_IOT_CHANNEL_ID,
        DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_IOT_CHANNEL_ID_STR,
        DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_WIFI_RSSI_SIGNAL_STRENGTH_IOT_ARRAY_OFFSET
    },
    {
        // Temperature
        DCI_IOT_TEMPERATURE_IOT_CHANNEL_ID,
        DCI_IOT_TEMPERATURE_IOT_CHANNEL_ID_STR,
        DCI_IOT_TEMPERATURE_DCID,
        IOT_DCI::CADENCE_GROUP3,
        DCI_IOT_TEMPERATURE_IOT_ARRAY_OFFSET
    },
    {
        // Power Up State
        DCI_IOT_POWER_UP_STATE_IOT_CHANNEL_ID,
        DCI_IOT_POWER_UP_STATE_IOT_CHANNEL_ID_STR,
        DCI_IOT_POWER_UP_STATE_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_POWER_UP_STATE_IOT_ARRAY_OFFSET
    },
    {
        // SBLCP Enable Disable
        DCI_IOT_SBLCP_EN_DIS_IOT_CHANNEL_ID,
        DCI_IOT_SBLCP_EN_DIS_IOT_CHANNEL_ID_STR,
        DCI_IOT_SBLCP_EN_DIS_DCID,
        IOT_DCI::CADENCE_GROUP2,
        DCI_IOT_SBLCP_EN_DIS_IOT_ARRAY_OFFSET
    },
};


//*******************************************************
//******     The IOT Cadence Groups table.
//*******************************************************

static const iot_cadence_group_struct_t iot_cadence_groups[IOT_DCI_DATA_TOTAL_GROUPS] =
{
    {
        // Group 0 Cadence - Normal SeeMe
        DCI_GROUP0_CADENCE_DCID,
        IOT_DCI::PUB_TYPE_SEEME,
        IOT_DCI::CADENCE_GROUP0,
        IOT_DCI::PUBLISH_ALL_ON_CONNECT
    },
    {
        // Group 1 Cadence - 1 minute StoreMe
        DCI_GROUP1_CADENCE_DCID,
        IOT_DCI::PUB_TYPE_STOREMEONINTERVAL,
        IOT_DCI::CADENCE_GROUP1,
        IOT_DCI::PUBLISH_ALL_ON_CONNECT
    },
    {
        // Group 2 Cadence - Fast SeeMe
        DCI_GROUP2_CADENCE_DCID,
        IOT_DCI::PUB_TYPE_SEEME,
        IOT_DCI::CADENCE_GROUP2,
        IOT_DCI::PUBLISH_ALL_ON_CONNECT
    },
    {
        // Group 3 Cadence - StoreMe on change
        DCI_GROUP3_CADENCE_DCID,
        IOT_DCI::PUB_TYPE_STOREME,
        IOT_DCI::CADENCE_GROUP3,
        IOT_DCI::PUBLISH_ALL_ON_CONNECT
    },
};


//*******************************************************
//******     The Target Definition Structure.
//*******************************************************

const iot_target_info_st_t iot_dci_data_target_info = 
{
    IOT_DCI_DATA_TOTAL_CHANNELS,
    iot_dci_data_iot_to_dcid,
    IOT_DCI_DATA_TOTAL_GROUPS,
    iot_cadence_groups
};

