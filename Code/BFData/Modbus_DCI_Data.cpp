/**
*****************************************************************************************
*   @file
*   @details See header file for module overview.
*   @copyright 2021 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#include "Includes.h"
#include "Modbus_DCI_Data.h"


//*******************************************************
//******     Modbus Object Defines.
//*******************************************************

static const DCI_ID_TD MODBUS_DEV_ID_VENDOR_ID = DCI_VENDOR_NAME_DCID;
static const DCI_ID_TD MODBUS_DEV_ID_PROD_CODE_ID = DCI_PRODUCT_CODE_ASCII_DCID;
static const DCI_ID_TD MODBUS_DEV_ID_FW_REV_ID = DCI_APP_FIRM_VER_NUM_DCID;
static const DCI_ID_TD MODBUS_DEV_ID_VENDOR_URL_ID = DCI_VENDOR_URL_DCID;
static const DCI_ID_TD MODBUS_DEV_ID_PROD_NAME_ID = DCI_PRODUCT_NAME_DCID;
static const DCI_ID_TD MODBUS_DEV_ID_MODEL_NAME_ID = DCI_MODEL_NAME_DCID;
static const DCI_ID_TD MODBUS_DEV_ID_USER_APP_NAME_ID = DCI_USER_APP_NAME_DCID;


//*******************************************************
//******     Modbus Register List
//*******************************************************

static const modbus_reg_t DCI_PRODUCT_SERIAL_NUM_MODBUS_REG = 0U;
static const modbus_reg_t DCI_VENDOR_NAME_MODBUS_REG = 2U;
static const modbus_reg_t DCI_PRODUCT_CODE_MODBUS_REG = 8U;
static const modbus_reg_t DCI_PRODUCT_CODE_ASCII_MODBUS_REG = 9U;
static const modbus_reg_t DCI_VENDOR_URL_MODBUS_REG = 13U;
static const modbus_reg_t DCI_PRODUCT_NAME_MODBUS_REG = 20U;
static const modbus_reg_t DCI_MODEL_NAME_MODBUS_REG = 36U;
static const modbus_reg_t DCI_USER_APP_NAME_MODBUS_REG = 41U;
static const modbus_reg_t DCI_APP_FIRM_VER_NUM_MODBUS_REG = 57U;
static const modbus_reg_t DCI_HARDWARE_VER_ASCII_MODBUS_REG = 59U;
static const modbus_reg_t DCI_HARDWARE_ID_VAL_MODBUS_REG = 66U;
static const modbus_reg_t DCI_CSTACK_USAGE_PERCENT_MODBUS_REG = 67U;
static const modbus_reg_t DCI_FIRMWARE_CRC_VAL_MODBUS_REG = 68U;
static const modbus_reg_t DCI_HEAP_USED_MAX_PERC_MODBUS_REG = 69U;
static const modbus_reg_t DCI_HEAP_USED_PERC_MODBUS_REG = 70U;
static const modbus_reg_t DCI_RESET_CAUSE_MODBUS_REG = 71U;
static const modbus_reg_t DCI_RESET_COUNTER_MODBUS_REG = 72U;
static const modbus_reg_t DCI_IDLE_LOOP_TIME_AVE_MODBUS_REG = 73U;
static const modbus_reg_t DCI_IDLE_LOOP_TIME_MAX_MODBUS_REG = 74U;
static const modbus_reg_t DCI_IDLE_LOOP_TIME_MIN_MODBUS_REG = 75U;
static const modbus_reg_t DCI_IP_ADDRESS_ALLOCATION_METHOD_MODBUS_REG = 76U;
static const modbus_reg_t DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_MODBUS_REG = 77U;
static const modbus_reg_t DCI_ACTIVE_IP_ADDRESS_MODBUS_REG = 78U;
static const modbus_reg_t DCI_ACTIVE_SUBNET_MASK_MODBUS_REG = 80U;
static const modbus_reg_t DCI_ACTIVE_DEFAULT_GATEWAY_MODBUS_REG = 82U;
static const modbus_reg_t DCI_STATIC_IP_ADDRESS_MODBUS_REG = 84U;
static const modbus_reg_t DCI_STATIC_SUBNET_MASK_MODBUS_REG = 86U;
static const modbus_reg_t DCI_STATIC_DEFAULT_GATEWAY_MODBUS_REG = 88U;
static const modbus_reg_t DCI_PHY1_LINK_SPEED_SELECT_MODBUS_REG = 90U;
static const modbus_reg_t DCI_PHY1_LINK_SPEED_ACTUAL_MODBUS_REG = 91U;
static const modbus_reg_t DCI_PHY1_FULL_DUPLEX_ENABLE_MODBUS_REG = 92U;
static const modbus_reg_t DCI_PHY1_FULL_DUPLEX_ACTUAL_MODBUS_REG = 93U;
static const modbus_reg_t DCI_PHY1_AUTONEG_ENABLE_MODBUS_REG = 94U;
static const modbus_reg_t DCI_PHY1_AUTONEG_STATE_MODBUS_REG = 95U;
static const modbus_reg_t DCI_PHY1_PORT_ENABLE_MODBUS_REG = 96U;
static const modbus_reg_t DCI_MULTICAST_ENABLE_MODBUS_REG = 97U;
static const modbus_reg_t DCI_BROADCAST_ENABLE_MODBUS_REG = 99U;
static const modbus_reg_t DCI_MODBUS_TCP_COMM_TIMEOUT_MODBUS_REG = 101U;
static const modbus_reg_t DCI_MODBUS_SERIAL_COMM_TIMEOUT_MODBUS_REG = 102U;
static const modbus_reg_t DCI_SIMPLE_APP_CONTROL_WORD_MODBUS_REG = 103U;
static const modbus_reg_t DCI_SIMPLE_APP_STATUS_WORD_MODBUS_REG = 104U;
static const modbus_reg_t DCI_LTK_VER_ASCII_MODBUS_REG = 105U;
static const modbus_reg_t DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_MODBUS_REG = 115U;
static const modbus_reg_t DCI_EXAMPLE_MODBUS_SERIAL_BAUD_MODBUS_REG = 116U;
static const modbus_reg_t DCI_EXAMPLE_MODBUS_SERIAL_PARITY_MODBUS_REG = 118U;
static const modbus_reg_t DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_MODBUS_REG = 119U;
static const modbus_reg_t DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_MODBUS_REG = 120U;
static const modbus_reg_t DCI_EIP_TEST1_MODBUS_REG = 121U;
static const modbus_reg_t DCI_EIP_TEST2_MODBUS_REG = 122U;
static const modbus_reg_t DCI_EIP_TEST3_MODBUS_REG = 123U;
static const modbus_reg_t DCI_EIP_TEST4_MODBUS_REG = 124U;
static const modbus_reg_t DCI_UNIX_EPOCH_TIME_MODBUS_REG = 125U;
static const modbus_reg_t DCI_RTC_TIME_MODBUS_REG = 127U;
static const modbus_reg_t DCI_LOG_INTERVAL_TIME_MS_MODBUS_REG = 129U;
static const modbus_reg_t DCI_CHANGE_BASED_LOGGING_MODBUS_REG = 131U;
static const modbus_reg_t DCI_MANUAL_LOG_TRIGGER_MODBUS_REG = 132U;
static const modbus_reg_t DCI_LOG_TEST_1_MODBUS_REG = 133U;
static const modbus_reg_t DCI_LOG_EVENT_TEST_101_MODBUS_REG = 135U;
static const modbus_reg_t DCI_LOG_EVENT_TEST_102_MODBUS_REG = 136U;
static const modbus_reg_t DCI_RESET_LINE_NUMBER_MODBUS_REG = 137U;
static const modbus_reg_t DCI_RESET_FUNCTION_NAME_MODBUS_REG = 139U;
static const modbus_reg_t DCI_RESET_OS_TASK_NAME_MODBUS_REG = 149U;
static const modbus_reg_t DCI_RESET_OS_TASK_PRIORITY_MODBUS_REG = 159U;
static const modbus_reg_t DCI_RESET_OS_STACK_SIZE_MODBUS_REG = 161U;
static const modbus_reg_t DCI_RESET_CR_TASK_MODBUS_REG = 163U;
static const modbus_reg_t DCI_RESET_CR_TASK_PARAM_MODBUS_REG = 165U;
static const modbus_reg_t DCI_RESET_CSTACK_SIZE_MODBUS_REG = 167U;
static const modbus_reg_t DCI_RESET_HIGHEST_OS_STACK_SIZE_MODBUS_REG = 169U;
static const modbus_reg_t DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG = 171U;
static const modbus_reg_t DCI_RESET_HEAP_USAGE_PERCENT_MODBUS_REG = 181U;
static const modbus_reg_t DCI_RESET_SOURCE_MODBUS_REG = 183U;
static const modbus_reg_t DCI_IOT_CONNECT_MODBUS_REG = 185U;
static const modbus_reg_t DCI_IOT_STATUS_MODBUS_REG = 186U;
static const modbus_reg_t DCI_IOT_UPDATE_RATE_MODBUS_REG = 187U;
static const modbus_reg_t DCI_IOT_CADENCE_UPDATE_RATE_MODBUS_REG = 188U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_1_VOLT_MODBUS_REG = 189U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_1_FREQ_MODBUS_REG = 190U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_1_PH_1_VOLT_MODBUS_REG = 191U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_1_PH_2_VOLT_MODBUS_REG = 192U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_1_PH_3_VOLT_MODBUS_REG = 193U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_2_VOLT_MODBUS_REG = 194U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_2_FREQ_MODBUS_REG = 195U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_2_PH_1_VOLT_MODBUS_REG = 196U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_2_PH_2_VOLT_MODBUS_REG = 197U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_2_PH_3_VOLT_MODBUS_REG = 198U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_ACT_PWR_MODBUS_REG = 199U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_APP_PWR_MODBUS_REG = 201U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_CURRENT_MODBUS_REG = 203U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_EFF_MODBUS_REG = 204U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_FREQ_MODBUS_REG = 205U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_VOLT_MODBUS_REG = 206U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_PH_1_PER_LOAD_MODBUS_REG = 207U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_PH_2_PER_LOAD_MODBUS_REG = 208U;
static const modbus_reg_t DCI_IOT_PWR_CONV_OUT_PH_3_PER_LOAD_MODBUS_REG = 209U;
static const modbus_reg_t DCI_IOT_PWR_CONV_TYPE_MODBUS_REG = 210U;
static const modbus_reg_t DCI_IOT_PWR_SUM_I_PART_NUM_MODBUS_REG = 211U;
static const modbus_reg_t DCI_IOT_PWR_SUM_VOLT_MODBUS_REG = 217U;
static const modbus_reg_t DCI_IOT_BATSYS_BAT_POL_1_VOLT_MODBUS_REG = 218U;
static const modbus_reg_t DCI_IOT_PWR_SUM_TEMP_MODBUS_REG = 219U;
static const modbus_reg_t DCI_IOT_PWR_SUM_RUNTIME_TO_EMP_MODBUS_REG = 220U;
static const modbus_reg_t DCI_IOT_PWR_SUM_REM_CAP_MODBUS_REG = 222U;
static const modbus_reg_t DCI_IOT_PWR_SUM_PER_LOAD_MODBUS_REG = 223U;
static const modbus_reg_t DCI_IOT_PWR_SUM_MODE_MODBUS_REG = 224U;
static const modbus_reg_t DCI_IOT_PWR_SUM_ALM_CODE_MODBUS_REG = 225U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_2_SWOFF_CON_MODBUS_REG = 226U;
static const modbus_reg_t DCI_IOT_PWR_CONV_IN_2_SWON_CON_MODBUS_REG = 227U;
static const modbus_reg_t DCI_IOT_OUTLSYS_OUTL_1_STAT_MODBUS_REG = 228U;
static const modbus_reg_t DCI_IOT_OUTLSYS_OUTL_2_STAT_MODBUS_REG = 229U;
static const modbus_reg_t DCI_IOT_OUTLSYS_OUTL_3_STAT_MODBUS_REG = 230U;
static const modbus_reg_t DCI_IOT_BATSYS_BAT_TEST_MODBUS_REG = 231U;
static const modbus_reg_t DCI_IOT_BATSYS_BAT_TEST_COM_MODBUS_REG = 232U;
static const modbus_reg_t DCI_IOT_BATSYS_BAT_ADTST_TST_W_DUR_MODBUS_REG = 233U;
static const modbus_reg_t DCI_IOT_BATSYS_BAT_ADTST_TST_W_LVL_MODBUS_REG = 234U;
static const modbus_reg_t DCI_IOT_CONN_STAT_REASON_MODBUS_REG = 235U;
static const modbus_reg_t DCI_PTP_SECONDS_TIME_MODBUS_REG = 236U;
static const modbus_reg_t DCI_PTP_NANOSECONDS_TIME_MODBUS_REG = 238U;

//*******************************************************
//******     The Modbus Register Counts.
//*******************************************************
static const modbus_reg_t MODBUS_DCI_DATA_TOTAL_REGISTERS = 240U;
static const DCI_ID_TD MODBUS_DCI_DATA_TOTAL_DCIDS = 116U;


//*******************************************************
//******     The Modbus Register Cross Reference Table.
//*******************************************************

static const DCI_MBUS_TO_DCID_LKUP_ST_TD modbus_dci_data_mbus_to_dcid[MODBUS_DCI_DATA_TOTAL_REGISTERS] = 
{
    {        // Device Product Serial Number + 0
        DCI_PRODUCT_SERIAL_NUM_MODBUS_REG,
        0U,
        DCI_PRODUCT_SERIAL_NUM_DCID
    }, 
    {        // Device Product Serial Number + 1
        DCI_PRODUCT_SERIAL_NUM_MODBUS_REG + 1U,
        2U,
        DCI_PRODUCT_SERIAL_NUM_DCID
    }, 
    {        // Product Vendor Name + 0
        DCI_VENDOR_NAME_MODBUS_REG,
        0U,
        DCI_VENDOR_NAME_DCID
    }, 
    {        // Product Vendor Name + 1
        DCI_VENDOR_NAME_MODBUS_REG + 1U,
        2U,
        DCI_VENDOR_NAME_DCID
    }, 
    {        // Product Vendor Name + 2
        DCI_VENDOR_NAME_MODBUS_REG + 2U,
        4U,
        DCI_VENDOR_NAME_DCID
    }, 
    {        // Product Vendor Name + 3
        DCI_VENDOR_NAME_MODBUS_REG + 3U,
        6U,
        DCI_VENDOR_NAME_DCID
    }, 
    {        // Product Vendor Name + 4
        DCI_VENDOR_NAME_MODBUS_REG + 4U,
        8U,
        DCI_VENDOR_NAME_DCID
    }, 
    {        // Product Vendor Name + 5
        DCI_VENDOR_NAME_MODBUS_REG + 5U,
        10U,
        DCI_VENDOR_NAME_DCID
    }, 
    {        // Product Code + 0
        DCI_PRODUCT_CODE_MODBUS_REG,
        0U,
        DCI_PRODUCT_CODE_DCID
    }, 
    {        // Product Code in ASCII + 0
        DCI_PRODUCT_CODE_ASCII_MODBUS_REG,
        0U,
        DCI_PRODUCT_CODE_ASCII_DCID
    }, 
    {        // Product Code in ASCII + 1
        DCI_PRODUCT_CODE_ASCII_MODBUS_REG + 1U,
        2U,
        DCI_PRODUCT_CODE_ASCII_DCID
    }, 
    {        // Product Code in ASCII + 2
        DCI_PRODUCT_CODE_ASCII_MODBUS_REG + 2U,
        4U,
        DCI_PRODUCT_CODE_ASCII_DCID
    }, 
    {        // Product Code in ASCII + 3
        DCI_PRODUCT_CODE_ASCII_MODBUS_REG + 3U,
        6U,
        DCI_PRODUCT_CODE_ASCII_DCID
    }, 
    {        // Corporate URL + 0
        DCI_VENDOR_URL_MODBUS_REG,
        0U,
        DCI_VENDOR_URL_DCID
    }, 
    {        // Corporate URL + 1
        DCI_VENDOR_URL_MODBUS_REG + 1U,
        2U,
        DCI_VENDOR_URL_DCID
    }, 
    {        // Corporate URL + 2
        DCI_VENDOR_URL_MODBUS_REG + 2U,
        4U,
        DCI_VENDOR_URL_DCID
    }, 
    {        // Corporate URL + 3
        DCI_VENDOR_URL_MODBUS_REG + 3U,
        6U,
        DCI_VENDOR_URL_DCID
    }, 
    {        // Corporate URL + 4
        DCI_VENDOR_URL_MODBUS_REG + 4U,
        8U,
        DCI_VENDOR_URL_DCID
    }, 
    {        // Corporate URL + 5
        DCI_VENDOR_URL_MODBUS_REG + 5U,
        10U,
        DCI_VENDOR_URL_DCID
    }, 
    {        // Corporate URL + 6
        DCI_VENDOR_URL_MODBUS_REG + 6U,
        12U,
        DCI_VENDOR_URL_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 0
        DCI_PRODUCT_NAME_MODBUS_REG,
        0U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 1
        DCI_PRODUCT_NAME_MODBUS_REG + 1U,
        2U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 2
        DCI_PRODUCT_NAME_MODBUS_REG + 2U,
        4U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 3
        DCI_PRODUCT_NAME_MODBUS_REG + 3U,
        6U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 4
        DCI_PRODUCT_NAME_MODBUS_REG + 4U,
        8U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 5
        DCI_PRODUCT_NAME_MODBUS_REG + 5U,
        10U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 6
        DCI_PRODUCT_NAME_MODBUS_REG + 6U,
        12U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 7
        DCI_PRODUCT_NAME_MODBUS_REG + 7U,
        14U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 8
        DCI_PRODUCT_NAME_MODBUS_REG + 8U,
        16U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 9
        DCI_PRODUCT_NAME_MODBUS_REG + 9U,
        18U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 10
        DCI_PRODUCT_NAME_MODBUS_REG + 10U,
        20U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 11
        DCI_PRODUCT_NAME_MODBUS_REG + 11U,
        22U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 12
        DCI_PRODUCT_NAME_MODBUS_REG + 12U,
        24U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 13
        DCI_PRODUCT_NAME_MODBUS_REG + 13U,
        26U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 14
        DCI_PRODUCT_NAME_MODBUS_REG + 14U,
        28U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Product Name - Short description of Prod Code + 15
        DCI_PRODUCT_NAME_MODBUS_REG + 15U,
        30U,
        DCI_PRODUCT_NAME_DCID
    }, 
    {        // Model Name + 0
        DCI_MODEL_NAME_MODBUS_REG,
        0U,
        DCI_MODEL_NAME_DCID
    }, 
    {        // Model Name + 1
        DCI_MODEL_NAME_MODBUS_REG + 1U,
        2U,
        DCI_MODEL_NAME_DCID
    }, 
    {        // Model Name + 2
        DCI_MODEL_NAME_MODBUS_REG + 2U,
        4U,
        DCI_MODEL_NAME_DCID
    }, 
    {        // Model Name + 3
        DCI_MODEL_NAME_MODBUS_REG + 3U,
        6U,
        DCI_MODEL_NAME_DCID
    }, 
    {        // Model Name + 4
        DCI_MODEL_NAME_MODBUS_REG + 4U,
        8U,
        DCI_MODEL_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 0
        DCI_USER_APP_NAME_MODBUS_REG,
        0U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 1
        DCI_USER_APP_NAME_MODBUS_REG + 1U,
        2U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 2
        DCI_USER_APP_NAME_MODBUS_REG + 2U,
        4U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 3
        DCI_USER_APP_NAME_MODBUS_REG + 3U,
        6U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 4
        DCI_USER_APP_NAME_MODBUS_REG + 4U,
        8U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 5
        DCI_USER_APP_NAME_MODBUS_REG + 5U,
        10U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 6
        DCI_USER_APP_NAME_MODBUS_REG + 6U,
        12U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 7
        DCI_USER_APP_NAME_MODBUS_REG + 7U,
        14U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 8
        DCI_USER_APP_NAME_MODBUS_REG + 8U,
        16U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 9
        DCI_USER_APP_NAME_MODBUS_REG + 9U,
        18U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 10
        DCI_USER_APP_NAME_MODBUS_REG + 10U,
        20U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 11
        DCI_USER_APP_NAME_MODBUS_REG + 11U,
        22U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 12
        DCI_USER_APP_NAME_MODBUS_REG + 12U,
        24U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 13
        DCI_USER_APP_NAME_MODBUS_REG + 13U,
        26U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 14
        DCI_USER_APP_NAME_MODBUS_REG + 14U,
        28U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // User Application Name or Device Tag + 15
        DCI_USER_APP_NAME_MODBUS_REG + 15U,
        30U,
        DCI_USER_APP_NAME_DCID
    }, 
    {        // Application Firmware Version Number + 0
        DCI_APP_FIRM_VER_NUM_MODBUS_REG,
        0U,
        DCI_APP_FIRM_VER_NUM_DCID
    }, 
    {        // Application Firmware Version Number + 1
        DCI_APP_FIRM_VER_NUM_MODBUS_REG + 1U,
        2U,
        DCI_APP_FIRM_VER_NUM_DCID
    }, 
    {        // Product Hardware Version + 0
        DCI_HARDWARE_VER_ASCII_MODBUS_REG,
        0U,
        DCI_HARDWARE_VER_ASCII_DCID
    }, 
    {        // Product Hardware Version + 1
        DCI_HARDWARE_VER_ASCII_MODBUS_REG + 1U,
        2U,
        DCI_HARDWARE_VER_ASCII_DCID
    }, 
    {        // Product Hardware Version + 2
        DCI_HARDWARE_VER_ASCII_MODBUS_REG + 2U,
        4U,
        DCI_HARDWARE_VER_ASCII_DCID
    }, 
    {        // Product Hardware Version + 3
        DCI_HARDWARE_VER_ASCII_MODBUS_REG + 3U,
        6U,
        DCI_HARDWARE_VER_ASCII_DCID
    }, 
    {        // Product Hardware Version + 4
        DCI_HARDWARE_VER_ASCII_MODBUS_REG + 4U,
        8U,
        DCI_HARDWARE_VER_ASCII_DCID
    }, 
    {        // Product Hardware Version + 5
        DCI_HARDWARE_VER_ASCII_MODBUS_REG + 5U,
        10U,
        DCI_HARDWARE_VER_ASCII_DCID
    }, 
    {        // Product Hardware Version + 6
        DCI_HARDWARE_VER_ASCII_MODBUS_REG + 6U,
        12U,
        DCI_HARDWARE_VER_ASCII_DCID
    }, 
    {        // Hardware ID Value + 0
        DCI_HARDWARE_ID_VAL_MODBUS_REG,
        0U,
        DCI_HARDWARE_ID_VAL_DCID
    }, 
    {        // Cstack Usage + 0
        DCI_CSTACK_USAGE_PERCENT_MODBUS_REG,
        0U,
        DCI_CSTACK_USAGE_PERCENT_DCID
    }, 
    {        // Firmware CRC + 0
        DCI_FIRMWARE_CRC_VAL_MODBUS_REG,
        0U,
        DCI_FIRMWARE_CRC_VAL_DCID
    }, 
    {        // Max Heap Stack Usage Percent + 0
        DCI_HEAP_USED_MAX_PERC_MODBUS_REG,
        0U,
        DCI_HEAP_USED_MAX_PERC_DCID
    }, 
    {        // Product Health - Heap Usage + 0
        DCI_HEAP_USED_PERC_MODBUS_REG,
        0U,
        DCI_HEAP_USED_PERC_DCID
    }, 
    {        // Reset Cause + 0
        DCI_RESET_CAUSE_MODBUS_REG,
        0U,
        DCI_RESET_CAUSE_DCID
    }, 
    {        // Reset Counter + 0
        DCI_RESET_COUNTER_MODBUS_REG,
        0U,
        DCI_RESET_COUNTER_DCID
    }, 
    {        // Idle Loop Time Average + 0
        DCI_IDLE_LOOP_TIME_AVE_MODBUS_REG,
        0U,
        DCI_IDLE_LOOP_TIME_AVE_DCID
    }, 
    {        // Idle Loop Time Max + 0
        DCI_IDLE_LOOP_TIME_MAX_MODBUS_REG,
        0U,
        DCI_IDLE_LOOP_TIME_MAX_DCID
    }, 
    {        // Idle Loop Time Min + 0
        DCI_IDLE_LOOP_TIME_MIN_MODBUS_REG,
        0U,
        DCI_IDLE_LOOP_TIME_MIN_DCID
    }, 
    {        // Allows to set IP Address Allocation Method + 0
        DCI_IP_ADDRESS_ALLOCATION_METHOD_MODBUS_REG,
        0U,
        DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID
    }, 
    {        // Shows the present IP Address Allocation Method + 0
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_MODBUS_REG,
        0U,
        DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_DCID
    }, 
    {        // Active IP Address + 0
        DCI_ACTIVE_IP_ADDRESS_MODBUS_REG,
        0U,
        DCI_ACTIVE_IP_ADDRESS_DCID
    }, 
    {        // Active IP Address + 1
        DCI_ACTIVE_IP_ADDRESS_MODBUS_REG + 1U,
        2U,
        DCI_ACTIVE_IP_ADDRESS_DCID
    }, 
    {        // Active Subnet Mask + 0
        DCI_ACTIVE_SUBNET_MASK_MODBUS_REG,
        0U,
        DCI_ACTIVE_SUBNET_MASK_DCID
    }, 
    {        // Active Subnet Mask + 1
        DCI_ACTIVE_SUBNET_MASK_MODBUS_REG + 1U,
        2U,
        DCI_ACTIVE_SUBNET_MASK_DCID
    }, 
    {        // Active Default Gateway + 0
        DCI_ACTIVE_DEFAULT_GATEWAY_MODBUS_REG,
        0U,
        DCI_ACTIVE_DEFAULT_GATEWAY_DCID
    }, 
    {        // Active Default Gateway + 1
        DCI_ACTIVE_DEFAULT_GATEWAY_MODBUS_REG + 1U,
        2U,
        DCI_ACTIVE_DEFAULT_GATEWAY_DCID
    }, 
    {        // Static IP Address + 0
        DCI_STATIC_IP_ADDRESS_MODBUS_REG,
        0U,
        DCI_STATIC_IP_ADDRESS_DCID
    }, 
    {        // Static IP Address + 1
        DCI_STATIC_IP_ADDRESS_MODBUS_REG + 1U,
        2U,
        DCI_STATIC_IP_ADDRESS_DCID
    }, 
    {        // Static IP Address + 0
        DCI_STATIC_SUBNET_MASK_MODBUS_REG,
        0U,
        DCI_STATIC_SUBNET_MASK_DCID
    }, 
    {        // Static IP Address + 1
        DCI_STATIC_SUBNET_MASK_MODBUS_REG + 1U,
        2U,
        DCI_STATIC_SUBNET_MASK_DCID
    }, 
    {        // Static Default Gateway + 0
        DCI_STATIC_DEFAULT_GATEWAY_MODBUS_REG,
        0U,
        DCI_STATIC_DEFAULT_GATEWAY_DCID
    }, 
    {        // Static Default Gateway + 1
        DCI_STATIC_DEFAULT_GATEWAY_MODBUS_REG + 1U,
        2U,
        DCI_STATIC_DEFAULT_GATEWAY_DCID
    }, 
    {        // Ethernet PHY 1 - Link Speed Select + 0
        DCI_PHY1_LINK_SPEED_SELECT_MODBUS_REG,
        0U,
        DCI_PHY1_LINK_SPEED_SELECT_DCID
    }, 
    {        // Ethernet PHY 1 - Link Speed Actual + 0
        DCI_PHY1_LINK_SPEED_ACTUAL_MODBUS_REG,
        0U,
        DCI_PHY1_LINK_SPEED_ACTUAL_DCID
    }, 
    {        // Ethernet PHY 1 - Duplex Select + 0
        DCI_PHY1_FULL_DUPLEX_ENABLE_MODBUS_REG,
        0U,
        DCI_PHY1_FULL_DUPLEX_ENABLE_DCID
    }, 
    {        // Ethernet PHY 1 - Duplex Actual + 0
        DCI_PHY1_FULL_DUPLEX_ACTUAL_MODBUS_REG,
        0U,
        DCI_PHY1_FULL_DUPLEX_ACTUAL_DCID
    }, 
    {        // Ethernet PHY 1 - Link Auto-Negotiate Enable + 0
        DCI_PHY1_AUTONEG_ENABLE_MODBUS_REG,
        0U,
        DCI_PHY1_AUTONEG_ENABLE_DCID
    }, 
    {        // Ethernet PHY 1 - Link Auto-Negotiate State + 0
        DCI_PHY1_AUTONEG_STATE_MODBUS_REG,
        0U,
        DCI_PHY1_AUTONEG_STATE_DCID
    }, 
    {        // Ethernet PHY 1 - Port Enable + 0
        DCI_PHY1_PORT_ENABLE_MODBUS_REG,
        0U,
        DCI_PHY1_PORT_ENABLE_DCID
    }, 
    {        // Multicast settings + 0
        DCI_MULTICAST_ENABLE_MODBUS_REG,
        0U,
        DCI_MULTICAST_ENABLE_DCID
    }, 
    {        // Multicast settings + 1
        DCI_MULTICAST_ENABLE_MODBUS_REG + 1U,
        2U,
        DCI_MULTICAST_ENABLE_DCID
    }, 
    {        // Broadcast settings + 0
        DCI_BROADCAST_ENABLE_MODBUS_REG,
        0U,
        DCI_BROADCAST_ENABLE_DCID
    }, 
    {        // Broadcast settings + 1
        DCI_BROADCAST_ENABLE_MODBUS_REG + 1U,
        2U,
        DCI_BROADCAST_ENABLE_DCID
    }, 
    {        // Modbus Comm Loss Timeout Value tcp(ms) + 0
        DCI_MODBUS_TCP_COMM_TIMEOUT_MODBUS_REG,
        0U,
        DCI_MODBUS_TCP_COMM_TIMEOUT_DCID
    }, 
    {        // Modbus Comm Loss Timeout Value for serial(ms) + 0
        DCI_MODBUS_SERIAL_COMM_TIMEOUT_MODBUS_REG,
        0U,
        DCI_MODBUS_SERIAL_COMM_TIMEOUT_DCID
    }, 
    {        // Switch to control LED + 0
        DCI_SIMPLE_APP_CONTROL_WORD_MODBUS_REG,
        0U,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID
    }, 
    {        // Switches to display the LED status + 0
        DCI_SIMPLE_APP_STATUS_WORD_MODBUS_REG,
        0U,
        DCI_SIMPLE_APP_STATUS_WORD_DCID
    }, 
    {        // LTK Revision ASCII + 0
        DCI_LTK_VER_ASCII_MODBUS_REG,
        0U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 1
        DCI_LTK_VER_ASCII_MODBUS_REG + 1U,
        2U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 2
        DCI_LTK_VER_ASCII_MODBUS_REG + 2U,
        4U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 3
        DCI_LTK_VER_ASCII_MODBUS_REG + 3U,
        6U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 4
        DCI_LTK_VER_ASCII_MODBUS_REG + 4U,
        8U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 5
        DCI_LTK_VER_ASCII_MODBUS_REG + 5U,
        10U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 6
        DCI_LTK_VER_ASCII_MODBUS_REG + 6U,
        12U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 7
        DCI_LTK_VER_ASCII_MODBUS_REG + 7U,
        14U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 8
        DCI_LTK_VER_ASCII_MODBUS_REG + 8U,
        16U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // LTK Revision ASCII + 9
        DCI_LTK_VER_ASCII_MODBUS_REG + 9U,
        18U,
        DCI_LTK_VER_ASCII_DCID
    }, 
    {        // Modbus serial example address + 0
        DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_MODBUS_REG,
        0U,
        DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_DCID
    }, 
    {        // modbus serial example baudrate + 0
        DCI_EXAMPLE_MODBUS_SERIAL_BAUD_MODBUS_REG,
        0U,
        DCI_EXAMPLE_MODBUS_SERIAL_BAUD_DCID
    }, 
    {        // modbus serial example baudrate + 1
        DCI_EXAMPLE_MODBUS_SERIAL_BAUD_MODBUS_REG + 1U,
        2U,
        DCI_EXAMPLE_MODBUS_SERIAL_BAUD_DCID
    }, 
    {        // modbus serial example parity + 0
        DCI_EXAMPLE_MODBUS_SERIAL_PARITY_MODBUS_REG,
        0U,
        DCI_EXAMPLE_MODBUS_SERIAL_PARITY_DCID
    }, 
    {        // modbus serial example stopbit + 0
        DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_MODBUS_REG,
        0U,
        DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_DCID
    }, 
    {        // DHCP to Static IP addr lock + 0
        DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_MODBUS_REG,
        0U,
        DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_DCID
    }, 
    {        // EIP Test Reg 1 + 0
        DCI_EIP_TEST1_MODBUS_REG,
        0U,
        DCI_EIP_TEST1_DCID
    }, 
    {        // EIP Test Reg 2 + 0
        DCI_EIP_TEST2_MODBUS_REG,
        0U,
        DCI_EIP_TEST2_DCID
    }, 
    {        // EIP Test Reg 3 + 0
        DCI_EIP_TEST3_MODBUS_REG,
        0U,
        DCI_EIP_TEST3_DCID
    }, 
    {        // EIP Test Reg 4 + 0
        DCI_EIP_TEST4_MODBUS_REG,
        0U,
        DCI_EIP_TEST4_DCID
    }, 
    {        // Unix Epoch Time + 0
        DCI_UNIX_EPOCH_TIME_MODBUS_REG,
        0U,
        DCI_UNIX_EPOCH_TIME_DCID
    }, 
    {        // Unix Epoch Time + 1
        DCI_UNIX_EPOCH_TIME_MODBUS_REG + 1U,
        2U,
        DCI_UNIX_EPOCH_TIME_DCID
    }, 
    {        // RTC Time + 0
        DCI_RTC_TIME_MODBUS_REG,
        0U,
        DCI_RTC_TIME_DCID
    }, 
    {        // RTC Time + 1
        DCI_RTC_TIME_MODBUS_REG + 1U,
        2U,
        DCI_RTC_TIME_DCID
    }, 
    {        // Logging Interval in miliseconds + 0
        DCI_LOG_INTERVAL_TIME_MS_MODBUS_REG,
        0U,
        DCI_LOG_INTERVAL_TIME_MS_DCID
    }, 
    {        // Logging Interval in miliseconds + 1
        DCI_LOG_INTERVAL_TIME_MS_MODBUS_REG + 1U,
        2U,
        DCI_LOG_INTERVAL_TIME_MS_DCID
    }, 
    {        // Change Based Logging + 0
        DCI_CHANGE_BASED_LOGGING_MODBUS_REG,
        0U,
        DCI_CHANGE_BASED_LOGGING_DCID
    }, 
    {        // Manual Trigger for logging + 0
        DCI_MANUAL_LOG_TRIGGER_MODBUS_REG,
        0U,
        DCI_MANUAL_LOG_TRIGGER_DCID
    }, 
    {        // Logging Test 1 + 0
        DCI_LOG_TEST_1_MODBUS_REG,
        0U,
        DCI_LOG_TEST_1_DCID
    }, 
    {        // Logging Test 1 + 1
        DCI_LOG_TEST_1_MODBUS_REG + 1U,
        2U,
        DCI_LOG_TEST_1_DCID
    }, 
    {        // Log Event 101 Test  + 0
        DCI_LOG_EVENT_TEST_101_MODBUS_REG,
        0U,
        DCI_LOG_EVENT_TEST_101_DCID
    }, 
    {        // Log Event 102 Test  + 0
        DCI_LOG_EVENT_TEST_102_MODBUS_REG,
        0U,
        DCI_LOG_EVENT_TEST_102_DCID
    }, 
    {        // Reset Line Number + 0
        DCI_RESET_LINE_NUMBER_MODBUS_REG,
        0U,
        DCI_RESET_LINE_NUMBER_DCID
    }, 
    {        // Reset Line Number + 1
        DCI_RESET_LINE_NUMBER_MODBUS_REG + 1U,
        2U,
        DCI_RESET_LINE_NUMBER_DCID
    }, 
    {        // Reset Function Name + 0
        DCI_RESET_FUNCTION_NAME_MODBUS_REG,
        0U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 1
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 1U,
        2U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 2
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 2U,
        4U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 3
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 3U,
        6U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 4
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 4U,
        8U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 5
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 5U,
        10U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 6
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 6U,
        12U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 7
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 7U,
        14U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 8
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 8U,
        16U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset Function Name + 9
        DCI_RESET_FUNCTION_NAME_MODBUS_REG + 9U,
        18U,
        DCI_RESET_FUNCTION_NAME_DCID
    }, 
    {        // Reset OS Task Name + 0
        DCI_RESET_OS_TASK_NAME_MODBUS_REG,
        0U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 1
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 1U,
        2U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 2
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 2U,
        4U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 3
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 3U,
        6U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 4
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 4U,
        8U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 5
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 5U,
        10U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 6
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 6U,
        12U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 7
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 7U,
        14U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 8
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 8U,
        16U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Name + 9
        DCI_RESET_OS_TASK_NAME_MODBUS_REG + 9U,
        18U,
        DCI_RESET_OS_TASK_NAME_DCID
    }, 
    {        // Reset OS Task Priority + 0
        DCI_RESET_OS_TASK_PRIORITY_MODBUS_REG,
        0U,
        DCI_RESET_OS_TASK_PRIORITY_DCID
    }, 
    {        // Reset OS Task Priority + 1
        DCI_RESET_OS_TASK_PRIORITY_MODBUS_REG + 1U,
        2U,
        DCI_RESET_OS_TASK_PRIORITY_DCID
    }, 
    {        // Reset OS Stack Size + 0
        DCI_RESET_OS_STACK_SIZE_MODBUS_REG,
        0U,
        DCI_RESET_OS_STACK_SIZE_DCID
    }, 
    {        // Reset OS Stack Size + 1
        DCI_RESET_OS_STACK_SIZE_MODBUS_REG + 1U,
        2U,
        DCI_RESET_OS_STACK_SIZE_DCID
    }, 
    {        // Reset CR Task + 0
        DCI_RESET_CR_TASK_MODBUS_REG,
        0U,
        DCI_RESET_CR_TASK_DCID
    }, 
    {        // Reset CR Task + 1
        DCI_RESET_CR_TASK_MODBUS_REG + 1U,
        2U,
        DCI_RESET_CR_TASK_DCID
    }, 
    {        // Reset CR Task Param + 0
        DCI_RESET_CR_TASK_PARAM_MODBUS_REG,
        0U,
        DCI_RESET_CR_TASK_PARAM_DCID
    }, 
    {        // Reset CR Task Param + 1
        DCI_RESET_CR_TASK_PARAM_MODBUS_REG + 1U,
        2U,
        DCI_RESET_CR_TASK_PARAM_DCID
    }, 
    {        // Reset Cstack Size + 0
        DCI_RESET_CSTACK_SIZE_MODBUS_REG,
        0U,
        DCI_RESET_CSTACK_SIZE_DCID
    }, 
    {        // Reset Cstack Size + 1
        DCI_RESET_CSTACK_SIZE_MODBUS_REG + 1U,
        2U,
        DCI_RESET_CSTACK_SIZE_DCID
    }, 
    {        // Reset Highest OS Stack Size + 0
        DCI_RESET_HIGHEST_OS_STACK_SIZE_MODBUS_REG,
        0U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_DCID
    }, 
    {        // Reset Highest OS Stack Size + 1
        DCI_RESET_HIGHEST_OS_STACK_SIZE_MODBUS_REG + 1U,
        2U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 0
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG,
        0U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 1
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 1U,
        2U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 2
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 2U,
        4U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 3
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 3U,
        6U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 4
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 4U,
        8U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 5
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 5U,
        10U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 6
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 6U,
        12U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 7
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 7U,
        14U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 8
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 8U,
        16U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Highest OS Stack Size Task + 9
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_MODBUS_REG + 9U,
        18U,
        DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_DCID
    }, 
    {        // Reset Heap Usage Percentage + 0
        DCI_RESET_HEAP_USAGE_PERCENT_MODBUS_REG,
        0U,
        DCI_RESET_HEAP_USAGE_PERCENT_DCID
    }, 
    {        // Reset Heap Usage Percentage + 1
        DCI_RESET_HEAP_USAGE_PERCENT_MODBUS_REG + 1U,
        2U,
        DCI_RESET_HEAP_USAGE_PERCENT_DCID
    }, 
    {        // Reset Source + 0
        DCI_RESET_SOURCE_MODBUS_REG,
        0U,
        DCI_RESET_SOURCE_DCID
    }, 
    {        // Reset Source + 1
        DCI_RESET_SOURCE_MODBUS_REG + 1U,
        2U,
        DCI_RESET_SOURCE_DCID
    }, 
    {        // IOT Enable or Disable + 0
        DCI_IOT_CONNECT_MODBUS_REG,
        0U,
        DCI_IOT_CONNECT_DCID
    }, 
    {        // IOT Connection Status + 0
        DCI_IOT_STATUS_MODBUS_REG,
        0U,
        DCI_IOT_STATUS_DCID
    }, 
    {        // IOT Data Update Rate + 0
        DCI_IOT_UPDATE_RATE_MODBUS_REG,
        0U,
        DCI_IOT_UPDATE_RATE_DCID
    }, 
    {        // IOT Cadence Update Rate + 0
        DCI_IOT_CADENCE_UPDATE_RATE_MODBUS_REG,
        0U,
        DCI_IOT_CADENCE_UPDATE_RATE_DCID
    }, 
    {        // DCI IOT PowerConverter_In_1_Volt + 0
        DCI_IOT_PWR_CONV_IN_1_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_1_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_In_1_Freq + 0
        DCI_IOT_PWR_CONV_IN_1_FREQ_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_1_FREQ_DCID
    }, 
    {        // DCI IOT PowerConverter_In_1_Ph_1_Volt + 0
        DCI_IOT_PWR_CONV_IN_1_PH_1_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_1_PH_1_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_In_1_Ph_2_Volt + 0
        DCI_IOT_PWR_CONV_IN_1_PH_2_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_1_PH_2_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_In_1_Ph_3_Volt + 0
        DCI_IOT_PWR_CONV_IN_1_PH_3_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_1_PH_3_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_In_2_Volt + 0
        DCI_IOT_PWR_CONV_IN_2_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_2_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_In_2_Freq + 0
        DCI_IOT_PWR_CONV_IN_2_FREQ_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_2_FREQ_DCID
    }, 
    {        // DCI IOT PowerConverter_In_2_Ph_1_Volt + 0
        DCI_IOT_PWR_CONV_IN_2_PH_1_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_2_PH_1_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_In_2_Ph_2_Volt + 0
        DCI_IOT_PWR_CONV_IN_2_PH_2_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_2_PH_2_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_In_2_Ph_3_Volt + 0
        DCI_IOT_PWR_CONV_IN_2_PH_3_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_2_PH_3_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Act_Power + 0
        DCI_IOT_PWR_CONV_OUT_ACT_PWR_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_ACT_PWR_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Act_Power + 1
        DCI_IOT_PWR_CONV_OUT_ACT_PWR_MODBUS_REG + 1U,
        2U,
        DCI_IOT_PWR_CONV_OUT_ACT_PWR_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_App_Power + 0
        DCI_IOT_PWR_CONV_OUT_APP_PWR_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_APP_PWR_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_App_Power + 1
        DCI_IOT_PWR_CONV_OUT_APP_PWR_MODBUS_REG + 1U,
        2U,
        DCI_IOT_PWR_CONV_OUT_APP_PWR_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Current + 0
        DCI_IOT_PWR_CONV_OUT_CURRENT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_CURRENT_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Eff + 0
        DCI_IOT_PWR_CONV_OUT_EFF_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_EFF_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Freq + 0
        DCI_IOT_PWR_CONV_OUT_FREQ_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_FREQ_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Volt + 0
        DCI_IOT_PWR_CONV_OUT_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_VOLT_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Ph_1_Per_Load + 0
        DCI_IOT_PWR_CONV_OUT_PH_1_PER_LOAD_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_PH_1_PER_LOAD_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Ph_2_Per_Load + 0
        DCI_IOT_PWR_CONV_OUT_PH_2_PER_LOAD_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_PH_2_PER_LOAD_DCID
    }, 
    {        // DCI IOT PowerConverter_Out_Ph_3_Per_Load + 0
        DCI_IOT_PWR_CONV_OUT_PH_3_PER_LOAD_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_OUT_PH_3_PER_LOAD_DCID
    }, 
    {        // DCI IOT PowerConverter_Type + 0
        DCI_IOT_PWR_CONV_TYPE_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_TYPE_DCID
    }, 
    {        // DCI IOT PowerSummary_iPartNumber + 0
        DCI_IOT_PWR_SUM_I_PART_NUM_MODBUS_REG,
        0U,
        DCI_IOT_PWR_SUM_I_PART_NUM_DCID
    }, 
    {        // DCI IOT PowerSummary_iPartNumber + 1
        DCI_IOT_PWR_SUM_I_PART_NUM_MODBUS_REG + 1U,
        2U,
        DCI_IOT_PWR_SUM_I_PART_NUM_DCID
    }, 
    {        // DCI IOT PowerSummary_iPartNumber + 2
        DCI_IOT_PWR_SUM_I_PART_NUM_MODBUS_REG + 2U,
        4U,
        DCI_IOT_PWR_SUM_I_PART_NUM_DCID
    }, 
    {        // DCI IOT PowerSummary_iPartNumber + 3
        DCI_IOT_PWR_SUM_I_PART_NUM_MODBUS_REG + 3U,
        6U,
        DCI_IOT_PWR_SUM_I_PART_NUM_DCID
    }, 
    {        // DCI IOT PowerSummary_iPartNumber + 4
        DCI_IOT_PWR_SUM_I_PART_NUM_MODBUS_REG + 4U,
        8U,
        DCI_IOT_PWR_SUM_I_PART_NUM_DCID
    }, 
    {        // DCI IOT PowerSummary_iPartNumber + 5
        DCI_IOT_PWR_SUM_I_PART_NUM_MODBUS_REG + 5U,
        10U,
        DCI_IOT_PWR_SUM_I_PART_NUM_DCID
    }, 
    {        // DCI IOT PowerSummary_Volt + 0
        DCI_IOT_PWR_SUM_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_PWR_SUM_VOLT_DCID
    }, 
    {        // DCI IOT BatterySys_Batt_Pole_1_Volt + 0
        DCI_IOT_BATSYS_BAT_POL_1_VOLT_MODBUS_REG,
        0U,
        DCI_IOT_BATSYS_BAT_POL_1_VOLT_DCID
    }, 
    {        // DCI IOT PowerSummary_Temp + 0
        DCI_IOT_PWR_SUM_TEMP_MODBUS_REG,
        0U,
        DCI_IOT_PWR_SUM_TEMP_DCID
    }, 
    {        // DCI IOT PowerSummary_RuntTimeToEmp + 0
        DCI_IOT_PWR_SUM_RUNTIME_TO_EMP_MODBUS_REG,
        0U,
        DCI_IOT_PWR_SUM_RUNTIME_TO_EMP_DCID
    }, 
    {        // DCI IOT PowerSummary_RuntTimeToEmp + 1
        DCI_IOT_PWR_SUM_RUNTIME_TO_EMP_MODBUS_REG + 1U,
        2U,
        DCI_IOT_PWR_SUM_RUNTIME_TO_EMP_DCID
    }, 
    {        // DCI IOT PowerSummary_RemCapacity + 0
        DCI_IOT_PWR_SUM_REM_CAP_MODBUS_REG,
        0U,
        DCI_IOT_PWR_SUM_REM_CAP_DCID
    }, 
    {        // DCI IOT PowerSummary_PercentLoad + 0
        DCI_IOT_PWR_SUM_PER_LOAD_MODBUS_REG,
        0U,
        DCI_IOT_PWR_SUM_PER_LOAD_DCID
    }, 
    {        // DCI IOT PowerSummary_Mode + 0
        DCI_IOT_PWR_SUM_MODE_MODBUS_REG,
        0U,
        DCI_IOT_PWR_SUM_MODE_DCID
    }, 
    {        // DCI IOT PowerSummary_Alarm_Code + 0
        DCI_IOT_PWR_SUM_ALM_CODE_MODBUS_REG,
        0U,
        DCI_IOT_PWR_SUM_ALM_CODE_DCID
    }, 
    {        // DCI IOT PowerConverter_In_2_SwOffControl + 0
        DCI_IOT_PWR_CONV_IN_2_SWOFF_CON_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_2_SWOFF_CON_DCID
    }, 
    {        // DCI IOT PowerConverter_In_2_SwOnControl + 0
        DCI_IOT_PWR_CONV_IN_2_SWON_CON_MODBUS_REG,
        0U,
        DCI_IOT_PWR_CONV_IN_2_SWON_CON_DCID
    }, 
    {        // DCI IOT_OutlSys_Outl_1_Status + 0
        DCI_IOT_OUTLSYS_OUTL_1_STAT_MODBUS_REG,
        0U,
        DCI_IOT_OUTLSYS_OUTL_1_STAT_DCID
    }, 
    {        // DCI IOT_OutlSys_Outl_2_Status + 0
        DCI_IOT_OUTLSYS_OUTL_2_STAT_MODBUS_REG,
        0U,
        DCI_IOT_OUTLSYS_OUTL_2_STAT_DCID
    }, 
    {        // DCI IOT_OutlSys_Outl_3_Status + 0
        DCI_IOT_OUTLSYS_OUTL_3_STAT_MODBUS_REG,
        0U,
        DCI_IOT_OUTLSYS_OUTL_3_STAT_DCID
    }, 
    {        // DCI IOT_BattSys_Bat_Test + 0
        DCI_IOT_BATSYS_BAT_TEST_MODBUS_REG,
        0U,
        DCI_IOT_BATSYS_BAT_TEST_DCID
    }, 
    {        // DCI IOT_BattSys_Bat_TestCommand + 0
        DCI_IOT_BATSYS_BAT_TEST_COM_MODBUS_REG,
        0U,
        DCI_IOT_BATSYS_BAT_TEST_COM_DCID
    }, 
    {        // DCI IOT_BattSys_Bat_AdTest_TestWDur + 0
        DCI_IOT_BATSYS_BAT_ADTST_TST_W_DUR_MODBUS_REG,
        0U,
        DCI_IOT_BATSYS_BAT_ADTST_TST_W_DUR_DCID
    }, 
    {        // DCI IOT_BattSys_Bat_AdTest_TestWLvl + 0
        DCI_IOT_BATSYS_BAT_ADTST_TST_W_LVL_MODBUS_REG,
        0U,
        DCI_IOT_BATSYS_BAT_ADTST_TST_W_LVL_DCID
    }, 
    {        // IOT Connection Status reason + 0
        DCI_IOT_CONN_STAT_REASON_MODBUS_REG,
        0U,
        DCI_IOT_CONN_STAT_REASON_DCID
    }, 
    {        // PTP Seconds timestamp + 0
        DCI_PTP_SECONDS_TIME_MODBUS_REG,
        0U,
        DCI_PTP_SECONDS_TIME_DCID
    }, 
    {        // PTP Seconds timestamp + 1
        DCI_PTP_SECONDS_TIME_MODBUS_REG + 1U,
        2U,
        DCI_PTP_SECONDS_TIME_DCID
    }, 
    {        // PTP Nanoseconds timestamp + 0
        DCI_PTP_NANOSECONDS_TIME_MODBUS_REG,
        0U,
        DCI_PTP_NANOSECONDS_TIME_DCID
    }, 
    {        // PTP Nanoseconds timestamp + 1
        DCI_PTP_NANOSECONDS_TIME_MODBUS_REG + 1U,
        2U,
        DCI_PTP_NANOSECONDS_TIME_DCID
    }, 
};


//The reverse lookup behavior was not needed so it was removed and replaced with a null.
static const DCI_MBUS_FROM_DCID_LKUP_ST_TD* const modbus_dci_data_dcid_to_mbus = nullptr;


//*******************************************************
//******     The Modbus Object Definition.
//*******************************************************

static const DCI_ID_TD modbus_dci_data_mbus_obj[MB_DEV_ID_MAX_OBJECTS] = 
    { MODBUS_DEV_ID_VENDOR_ID, MODBUS_DEV_ID_PROD_CODE_ID, MODBUS_DEV_ID_FW_REV_ID, 
        MODBUS_DEV_ID_VENDOR_URL_ID, MODBUS_DEV_ID_PROD_NAME_ID, MODBUS_DEV_ID_MODEL_NAME_ID, 
        MODBUS_DEV_ID_USER_APP_NAME_ID };


//*******************************************************
//******     The Modbus Target Definition Structure.
//*******************************************************

const MODBUS_TARGET_INFO_ST_TD modbus_dci_data_target_info =
{
    MB_DEV_ID_MAX_OBJECTS,        //uint8_t total_mbus_objs
    modbus_dci_data_mbus_obj,        //const DCI_ID_TD* mbus_obj_list
    MODBUS_DCI_DATA_TOTAL_REGISTERS,        //MODBUS_REG_TD total_registers
    modbus_dci_data_mbus_to_dcid,        //const DCI_MBUS_TO_DCID_LKUP_ST_TD* mbus_to_dcid
    MODBUS_DCI_DATA_TOTAL_DCIDS,        //DCI_ID_TD total_dcids
    modbus_dci_data_dcid_to_mbus        //const DCI_MBUS_FROM_DCID_LKUP_ST_TD* dcid_to_mbus
};


