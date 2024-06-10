/**
*****************************************************************************************
*   @file
*   @details See header file for module overview.
*   @copyright 2020 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#include "Includes.h"
#include "EIP_DCI_Data.h"



//*******************************************************
//******     Identity Object Device Type Definition
//*******************************************************

static const uint16_t EIP_DCI_DATA_DEVICE_TYPE =                 0x2BU;


//*******************************************************
//******     Class, Instance, Attribute Item Counts
//*******************************************************

///Total number of classes.
static const uint16_t EIP_DCI_DATA_TOTAL_CLASSES=                8U;

///Total number of attributes and instances per class.
static const uint8_t EIP_DCI_DATA_CLASS_0x01_NUM_INSTANCES =     1U;
static const uint8_t EIP_DCI_DATA_CLASS_0x01_NUM_ATTRIBUTES =    7U;
static const uint8_t EIP_DCI_DATA_CLASS_0x04_NUM_INSTANCES =     4U;
static const uint8_t EIP_DCI_DATA_CLASS_0x04_NUM_ATTRIBUTES =    3U;
static const uint8_t EIP_DCI_DATA_CLASS_0x07_NUM_INSTANCES =     2U;
static const uint8_t EIP_DCI_DATA_CLASS_0x07_NUM_ATTRIBUTES =    1U;
static const uint8_t EIP_DCI_DATA_CLASS_0x08_NUM_INSTANCES =     5U;
static const uint8_t EIP_DCI_DATA_CLASS_0x08_NUM_ATTRIBUTES =    1U;
static const uint8_t EIP_DCI_DATA_CLASS_0x09_NUM_INSTANCES =     5U;
static const uint8_t EIP_DCI_DATA_CLASS_0x09_NUM_ATTRIBUTES =    1U;
static const uint8_t EIP_DCI_DATA_CLASS_0x96_NUM_INSTANCES =     1U;
static const uint8_t EIP_DCI_DATA_CLASS_0x96_NUM_ATTRIBUTES =    8U;
static const uint8_t EIP_DCI_DATA_CLASS_0x97_NUM_INSTANCES =     1U;
static const uint8_t EIP_DCI_DATA_CLASS_0x97_NUM_ATTRIBUTES =    4U;
static const uint8_t EIP_DCI_DATA_CLASS_0xA0_NUM_INSTANCES =     1U;
static const uint8_t EIP_DCI_DATA_CLASS_0xA0_NUM_ATTRIBUTES =    8U;


//*******************************************************
//******     Define Attribute Structure Lists
//*******************************************************

const CIP_ATTRIB_ST_TD eip_dci_data_C0x01_INST_0x01_attrib_list[EIP_DCI_DATA_CLASS_0x01_NUM_ATTRIBUTES] = 
{
    {        // C: 0x01, I: 0x01, A: 0x01 - Eaton Vendor ID Attrib    0x01
        0x01U,
        DCI_CIP_VENDOR_ID_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x01, I: 0x01, A: 0x02 - CIP Device Type Attrib    0x02
        0x02U,
        DCI_CIP_DEVICE_TYPE_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x01, I: 0x01, A: 0x03 - Product Code Attrib    0x03
        0x03U,
        DCI_PRODUCT_CODE_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x01, I: 0x01, A: 0x04 - Application Firmware Version Attrib    0x04
        0x04U,
        DCI_APP_FIRM_VER_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x01, I: 0x01, A: 0x05 - CIP Device Identity Device Status Attrib    0x05
        0x05U,
        DCI_CIP_DEVICE_IDENT_DEV_STATUS_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x01, I: 0x01, A: 0x06 - Device Product Serial Number Attrib    0x06
        0x06U,
        DCI_PRODUCT_SERIAL_NUM_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x01, I: 0x01, A: 0x07 - Product Name - Short description of Prod Code Attrib    0x07
        0x07U,
        DCI_PRODUCT_NAME_DCID,
        0U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_SHORT_STRING_DTYPE )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x04_INST_0x64_attrib_list[EIP_DCI_DATA_CLASS_0x04_NUM_ATTRIBUTES] = 
{
    {        // C: 0x04, I: 0x64, A: 0x03 - EIP input assembly test Attrib    0x03
        0x03U,
        DCI_EIP_TEST_INPUT_ASM_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x04_INST_0x78_attrib_list[EIP_DCI_DATA_CLASS_0x04_NUM_ATTRIBUTES] = 
{
    {        // C: 0x04, I: 0x78, A: 0x03 - EIP output assembly test Attrib    0x03
        0x03U,
        DCI_EIP_TEST_OUTPUT_ASM_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x04_INST_0x96_attrib_list[EIP_DCI_DATA_CLASS_0x04_NUM_ATTRIBUTES] = 
{
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p assembly (Instance 150) Attrib    0x03
        0x03U,
        DCI_DYN_IN_ASM_INTERFACE_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x04_INST_0x97_attrib_list[EIP_DCI_DATA_CLASS_0x04_NUM_ATTRIBUTES] = 
{
    {        // C: 0x04, I: 0x97, A: 0x03 - Dynamic o/p assembly Attrib    0x03
        0x03U,
        DCI_DYN_OUT_ASM_INTERFACE_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x07_INST_0x01_attrib_list[EIP_DCI_DATA_CLASS_0x07_NUM_ATTRIBUTES] = 
{
    {        // C: 0x07, I: 0x01, A: 0x04 - simple application control word Attrib    0x04
        0x04U,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x07_INST_0x02_attrib_list[EIP_DCI_DATA_CLASS_0x07_NUM_ATTRIBUTES] = 
{
    {        // C: 0x07, I: 0x02, A: 0x04 - simple application status word Attrib    0x04
        0x04U,
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x08_INST_0x01_attrib_list[EIP_DCI_DATA_CLASS_0x08_NUM_ATTRIBUTES] = 
{
    {        // C: 0x08, I: 0x01, A: 0x03 - simple application status word Attrib    0x03
        0x03U,
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        0U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN ) |
            static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x08_INST_0x02_attrib_list[EIP_DCI_DATA_CLASS_0x08_NUM_ATTRIBUTES] = 
{
    {        // C: 0x08, I: 0x02, A: 0x03 - simple application status word Attrib    0x03
        0x03U,
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        1U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN ) |
            static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x08_INST_0x03_attrib_list[EIP_DCI_DATA_CLASS_0x08_NUM_ATTRIBUTES] = 
{
    {        // C: 0x08, I: 0x03, A: 0x03 - simple application status word Attrib    0x03
        0x03U,
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        2U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN ) |
            static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x08_INST_0x04_attrib_list[EIP_DCI_DATA_CLASS_0x08_NUM_ATTRIBUTES] = 
{
    {        // C: 0x08, I: 0x04, A: 0x03 - simple application status word Attrib    0x03
        0x03U,
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        3U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN ) |
            static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x08_INST_0x05_attrib_list[EIP_DCI_DATA_CLASS_0x08_NUM_ATTRIBUTES] = 
{
    {        // C: 0x08, I: 0x05, A: 0x03 - Input Test Bit 1 Attrib    0x03
        0x03U,
        DCI_INPUT_TEST_BIT_1_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x09_INST_0x01_attrib_list[EIP_DCI_DATA_CLASS_0x09_NUM_ATTRIBUTES] = 
{
    {        // C: 0x09, I: 0x01, A: 0x03 - simple application control word Attrib    0x03
        0x03U,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        0U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN ) |
            static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x09_INST_0x02_attrib_list[EIP_DCI_DATA_CLASS_0x09_NUM_ATTRIBUTES] = 
{
    {        // C: 0x09, I: 0x02, A: 0x03 - simple application control word Attrib    0x03
        0x03U,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        1U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN ) |
            static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x09_INST_0x03_attrib_list[EIP_DCI_DATA_CLASS_0x09_NUM_ATTRIBUTES] = 
{
    {        // C: 0x09, I: 0x03, A: 0x03 - simple application control word Attrib    0x03
        0x03U,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        2U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN ) |
            static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x09_INST_0x04_attrib_list[EIP_DCI_DATA_CLASS_0x09_NUM_ATTRIBUTES] = 
{
    {        // C: 0x09, I: 0x04, A: 0x03 - simple application control word Attrib    0x03
        0x03U,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        3U,
        0U,
        ( 
            static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN ) |
            static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x09_INST_0x05_attrib_list[EIP_DCI_DATA_CLASS_0x09_NUM_ATTRIBUTES] = 
{
    {        // C: 0x09, I: 0x05, A: 0x03 - Output Test Bit 1 Attrib    0x03
        0x03U,
        DCI_OUTPUT_TEST_BIT_1_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x96_INST_0x01_attrib_list[EIP_DCI_DATA_CLASS_0x96_NUM_ATTRIBUTES] = 
{
    {        // C: 0x96, I: 0x01, A: 0x01 - Dynamic i/p Assembly (Instance 150) Selection1 Attrib    0x01
        0x01U,
        DCI_DYN_IN_ASM_SELECT_1_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x96, I: 0x01, A: 0x02 - Dynamic i/p Assembly (Instance 150) Selection2 Attrib    0x02
        0x02U,
        DCI_DYN_IN_ASM_SELECT_2_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x96, I: 0x01, A: 0x03 - Dynamic i/p Assembly (Instance 150) Selection3 Attrib    0x03
        0x03U,
        DCI_DYN_IN_ASM_SELECT_3_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x96, I: 0x01, A: 0x04 - Dynamic i/p Assembly (Instance 150) Selection4 Attrib    0x04
        0x04U,
        DCI_DYN_IN_ASM_SELECT_4_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x96, I: 0x01, A: 0x05 - Dynamic i/p Assembly (Instance 150) Selection5 Attrib    0x05
        0x05U,
        DCI_DYN_IN_ASM_SELECT_5_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x96, I: 0x01, A: 0x06 - Dynamic i/p Assembly (Instance 150) Selection6 Attrib    0x06
        0x06U,
        DCI_DYN_IN_ASM_SELECT_6_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x96, I: 0x01, A: 0x07 - Dynamic i/p Assembly (Instance 150) Selection7 Attrib    0x07
        0x07U,
        DCI_DYN_IN_ASM_SELECT_7_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x96, I: 0x01, A: 0x08 - Dynamic i/p Assembly (Instance 150) Selection8 Attrib    0x08
        0x08U,
        DCI_DYN_IN_ASM_SELECT_8_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0x97_INST_0x01_attrib_list[EIP_DCI_DATA_CLASS_0x97_NUM_ATTRIBUTES] = 
{
    {        // C: 0x97, I: 0x01, A: 0x01 - Dynamic o/p Assembly (Instance 150) Selection1 Attrib    0x01
        0x01U,
        DCI_DYN_OUT_ASM_SELECT_1_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x97, I: 0x01, A: 0x02 - Dynamic o/p Assembly (Instance 150) Selection2 Attrib    0x02
        0x02U,
        DCI_DYN_OUT_ASM_SELECT_2_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x97, I: 0x01, A: 0x03 - Dynamic o/p Assembly (Instance 150) Selection3 Attrib    0x03
        0x03U,
        DCI_DYN_OUT_ASM_SELECT_3_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0x97, I: 0x01, A: 0x04 - Dynamic o/p Assembly (Instance 150) Selection4 Attrib    0x04
        0x04U,
        DCI_DYN_OUT_ASM_SELECT_4_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
};

const CIP_ATTRIB_ST_TD eip_dci_data_C0xA0_INST_0x01_attrib_list[EIP_DCI_DATA_CLASS_0xA0_NUM_ATTRIBUTES] = 
{
    {        // C: 0xA0, I: 0x01, A: 0x01 - EIP Test Reg 1 Attrib    0x01
        0x01U,
        DCI_EIP_TEST1_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0xA0, I: 0x01, A: 0x02 - EIP Test Reg 2 Attrib    0x02
        0x02U,
        DCI_EIP_TEST2_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0xA0, I: 0x01, A: 0x03 - EIP Test Reg 3 Attrib    0x03
        0x03U,
        DCI_EIP_TEST3_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0xA0, I: 0x01, A: 0x04 - simple application status word Attrib    0x04
        0x04U,
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0xA0, I: 0x01, A: 0x05 - Product Code Attrib    0x05
        0x05U,
        DCI_PRODUCT_CODE_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0xA0, I: 0x01, A: 0x06 - Reset Counter Attrib    0x06
        0x06U,
        DCI_RESET_COUNTER_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0xA0, I: 0x01, A: 0x07 - Ethernet PHY 1 - Link Speed Actual Attrib    0x07
        0x07U,
        DCI_PHY1_LINK_SPEED_ACTUAL_DCID,
        0U,
        0U,
        ( 
            0U
        )
    },
    {        // C: 0xA0, I: 0x01, A: 0x08 - EIP Test Reg 4 Attrib    0x08
        0x08U,
        DCI_EIP_TEST4_DCID,
        0U,
        0U,
        ( 
            0U
        )
    }
};


//*******************************************************
//******     Define Instance Structure Lists
//*******************************************************

const CIP_INSTANCE_ST_TD eip_dci_data_C0x01_instance_list[EIP_DCI_DATA_CLASS_0x01_NUM_INSTANCES] = 
{
    {
        1U,
        eip_dci_data_C0x01_INST_0x01_attrib_list
    },
};

const CIP_INSTANCE_ST_TD eip_dci_data_C0x04_instance_list[EIP_DCI_DATA_CLASS_0x04_NUM_INSTANCES] = 
{
    {
        100U,
        eip_dci_data_C0x04_INST_0x64_attrib_list
    },
    {
        120U,
        eip_dci_data_C0x04_INST_0x78_attrib_list
    },
    {
        150U,
        eip_dci_data_C0x04_INST_0x96_attrib_list
    },
    {
        151U,
        eip_dci_data_C0x04_INST_0x97_attrib_list
    },
};

const CIP_INSTANCE_ST_TD eip_dci_data_C0x07_instance_list[EIP_DCI_DATA_CLASS_0x07_NUM_INSTANCES] = 
{
    {
        1U,
        eip_dci_data_C0x07_INST_0x01_attrib_list
    },
    {
        2U,
        eip_dci_data_C0x07_INST_0x02_attrib_list
    },
};

const CIP_INSTANCE_ST_TD eip_dci_data_C0x08_instance_list[EIP_DCI_DATA_CLASS_0x08_NUM_INSTANCES] = 
{
    {
        1U,
        eip_dci_data_C0x08_INST_0x01_attrib_list
    },
    {
        2U,
        eip_dci_data_C0x08_INST_0x02_attrib_list
    },
    {
        3U,
        eip_dci_data_C0x08_INST_0x03_attrib_list
    },
    {
        4U,
        eip_dci_data_C0x08_INST_0x04_attrib_list
    },
    {
        5U,
        eip_dci_data_C0x08_INST_0x05_attrib_list
    },
};

const CIP_INSTANCE_ST_TD eip_dci_data_C0x09_instance_list[EIP_DCI_DATA_CLASS_0x09_NUM_INSTANCES] = 
{
    {
        1U,
        eip_dci_data_C0x09_INST_0x01_attrib_list
    },
    {
        2U,
        eip_dci_data_C0x09_INST_0x02_attrib_list
    },
    {
        3U,
        eip_dci_data_C0x09_INST_0x03_attrib_list
    },
    {
        4U,
        eip_dci_data_C0x09_INST_0x04_attrib_list
    },
    {
        5U,
        eip_dci_data_C0x09_INST_0x05_attrib_list
    },
};

const CIP_INSTANCE_ST_TD eip_dci_data_C0x96_instance_list[EIP_DCI_DATA_CLASS_0x96_NUM_INSTANCES] = 
{
    {
        1U,
        eip_dci_data_C0x96_INST_0x01_attrib_list
    },
};

const CIP_INSTANCE_ST_TD eip_dci_data_C0x97_instance_list[EIP_DCI_DATA_CLASS_0x97_NUM_INSTANCES] = 
{
    {
        1U,
        eip_dci_data_C0x97_INST_0x01_attrib_list
    },
};

const CIP_INSTANCE_ST_TD eip_dci_data_C0xA0_instance_list[EIP_DCI_DATA_CLASS_0xA0_NUM_INSTANCES] = 
{
    {
        1U,
        eip_dci_data_C0xA0_INST_0x01_attrib_list
    },
};


//*******************************************************
//******      Define the Class Structure List
//*******************************************************

const CIP_CLASS_ST_TD eip_dci_data_cip_class_list[EIP_DCI_DATA_TOTAL_CLASSES] = 
{
    {        // C: 1 Index: 0
        0x01U,
        eip_dci_data_C0x01_instance_list,
        1U,
        EIP_DCI_DATA_CLASS_0x01_NUM_INSTANCES,
        EIP_DCI_DATA_CLASS_0x01_NUM_ATTRIBUTES
    },
    {        // C: 4 Index: 1
        0x04U,
        eip_dci_data_C0x04_instance_list,
        1U,
        EIP_DCI_DATA_CLASS_0x04_NUM_INSTANCES,
        EIP_DCI_DATA_CLASS_0x04_NUM_ATTRIBUTES
    },
    {        // C: 7 Index: 2
        0x07U,
        eip_dci_data_C0x07_instance_list,
        1U,
        EIP_DCI_DATA_CLASS_0x07_NUM_INSTANCES,
        EIP_DCI_DATA_CLASS_0x07_NUM_ATTRIBUTES
    },
    {        // C: 8 Index: 3
        0x08U,
        eip_dci_data_C0x08_instance_list,
        2U,
        EIP_DCI_DATA_CLASS_0x08_NUM_INSTANCES,
        EIP_DCI_DATA_CLASS_0x08_NUM_ATTRIBUTES
    },
    {        // C: 9 Index: 4
        0x09U,
        eip_dci_data_C0x09_instance_list,
        1U,
        EIP_DCI_DATA_CLASS_0x09_NUM_INSTANCES,
        EIP_DCI_DATA_CLASS_0x09_NUM_ATTRIBUTES
    },
    {        // C: 150 Index: 5
        0x96U,
        eip_dci_data_C0x96_instance_list,
        1U,
        EIP_DCI_DATA_CLASS_0x96_NUM_INSTANCES,
        EIP_DCI_DATA_CLASS_0x96_NUM_ATTRIBUTES
    },
    {        // C: 151 Index: 6
        0x97U,
        eip_dci_data_C0x97_instance_list,
        1U,
        EIP_DCI_DATA_CLASS_0x97_NUM_INSTANCES,
        EIP_DCI_DATA_CLASS_0x97_NUM_ATTRIBUTES
    },
    {        // C: 160 Index: 7
        0xA0U,
        eip_dci_data_C0xA0_instance_list,
        1U,
        EIP_DCI_DATA_CLASS_0xA0_NUM_INSTANCES,
        EIP_DCI_DATA_CLASS_0xA0_NUM_ATTRIBUTES
    },
};


//*******************************************************
//******     Message Router Object Attribute 1 Definition
//*******************************************************

const uint16_t eip_dci_data_msg_rtr_attrib1[EIP_DCI_DATA_TOTAL_CLASSES + 1U] = 
{
    EIP_DCI_DATA_TOTAL_CLASSES,
    0x0001U,
    0x0004U,
    0x0007U,
    0x0008U,
    0x0009U,
    0x0096U,
    0x0097U,
    0x00A0U,
};




//*******************************************************
//******     The EIP Assembly Object Definition.
//*******************************************************

static const uint8_t DCI_EIP_TEST_INPUT_ASM_INST_0x64_ASM_DCID_LIST_LEN =     2U;
const DCI_ID_TD DCI_EIP_TEST_INPUT_ASM_INST_0x64_ASM_DCID_LIST[DCI_EIP_TEST_INPUT_ASM_INST_0x64_ASM_DCID_LIST_LEN] =
{
    DCI_SIMPLE_APP_STATUS_WORD_DCID,
    DCI_CIP_VENDOR_ID_DCID,
};
static const uint8_t DCI_EIP_TEST_INPUT_ASM_INST_0x64_ASM_OFFSET =    2U;    //This is the alignment size for this assembly head.
static const uint8_t DCI_SIMPLE_APP_STATUS_WORD_INST_0x64_ASM_OFFSET=    0U;
static const uint8_t DCI_CIP_VENDOR_ID_INST_0x64_ASM_OFFSET=     2U;
static const uint8_t DCI_EIP_TEST_OUTPUT_ASM_INST_0x78_ASM_DCID_LIST_LEN =     2U;
const DCI_ID_TD DCI_EIP_TEST_OUTPUT_ASM_INST_0x78_ASM_DCID_LIST[DCI_EIP_TEST_OUTPUT_ASM_INST_0x78_ASM_DCID_LIST_LEN] =
{
    DCI_SIMPLE_APP_CONTROL_WORD_DCID,
    DCI_EIP_TEST1_DCID,
};
static const uint8_t DCI_EIP_TEST_OUTPUT_ASM_INST_0x78_ASM_OFFSET =    2U;    //This is the alignment size for this assembly head.
static const uint8_t DCI_SIMPLE_APP_CONTROL_WORD_INST_0x78_ASM_OFFSET=    0U;
static const uint8_t DCI_EIP_TEST1_INST_0x78_ASM_OFFSET=         2U;
static const uint8_t DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_DCID_LIST_LEN =     8U;
const DCI_ID_TD DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_DCID_LIST[DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_DCID_LIST_LEN] =
{
    DCI_DYN_IN_ASM_MEMBER_1_DCID,
    DCI_DYN_IN_ASM_MEMBER_2_DCID,
    DCI_DYN_IN_ASM_MEMBER_3_DCID,
    DCI_DYN_IN_ASM_MEMBER_4_DCID,
    DCI_DYN_IN_ASM_MEMBER_5_DCID,
    DCI_DYN_IN_ASM_MEMBER_6_DCID,
    DCI_DYN_IN_ASM_MEMBER_7_DCID,
    DCI_DYN_IN_ASM_MEMBER_8_DCID,
};
static const uint8_t DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_OFFSET =    1U;    //This is the alignment size for this assembly head.
static const uint8_t DCI_DYN_IN_ASM_MEMBER_1_INST_0x96_ASM_OFFSET=    0U;
static const uint8_t DCI_DYN_IN_ASM_MEMBER_2_INST_0x96_ASM_OFFSET=    4U;
static const uint8_t DCI_DYN_IN_ASM_MEMBER_3_INST_0x96_ASM_OFFSET=    8U;
static const uint8_t DCI_DYN_IN_ASM_MEMBER_4_INST_0x96_ASM_OFFSET=    12U;
static const uint8_t DCI_DYN_IN_ASM_MEMBER_5_INST_0x96_ASM_OFFSET=    16U;
static const uint8_t DCI_DYN_IN_ASM_MEMBER_6_INST_0x96_ASM_OFFSET=    20U;
static const uint8_t DCI_DYN_IN_ASM_MEMBER_7_INST_0x96_ASM_OFFSET=    24U;
static const uint8_t DCI_DYN_IN_ASM_MEMBER_8_INST_0x96_ASM_OFFSET=    28U;
static const uint8_t DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_DCID_LIST_LEN =     4U;
const DCI_ID_TD DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_DCID_LIST[DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_DCID_LIST_LEN] =
{
    DCI_DYN_OUT_ASM_MEMBER_1_DCID,
    DCI_DYN_OUT_ASM_MEMBER_2_DCID,
    DCI_DYN_OUT_ASM_MEMBER_3_DCID,
    DCI_DYN_OUT_ASM_MEMBER_4_DCID,
};
static const uint8_t DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_OFFSET =    1U;    //This is the alignment size for this assembly head.
static const uint8_t DCI_DYN_OUT_ASM_MEMBER_1_INST_0x97_ASM_OFFSET=    0U;
static const uint8_t DCI_DYN_OUT_ASM_MEMBER_2_INST_0x97_ASM_OFFSET=    4U;
static const uint8_t DCI_DYN_OUT_ASM_MEMBER_3_INST_0x97_ASM_OFFSET=    8U;
static const uint8_t DCI_DYN_OUT_ASM_MEMBER_4_INST_0x97_ASM_OFFSET=    12U;

static const uint16_t EIP_DCI_DATA_TOTAL_ASM_ITEMS =             20U;

const CIP_ASM_STRUCT_TD eip_dci_data_dcid_to_asm[EIP_DCI_DATA_TOTAL_ASM_ITEMS] = 
{
    {        // C: 0x04, I: 0x78, A: 0x03 - simple application control word
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        120U,
        false,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_SIMPLE_APP_CONTROL_WORD_INST_0x78_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x64, A: 0x03 - simple application status word
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        100U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_SIMPLE_APP_STATUS_WORD_INST_0x64_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x64, A: 0x03 - Eaton Vendor ID
        DCI_CIP_VENDOR_ID_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        100U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_CIP_VENDOR_ID_INST_0x64_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x64, A: 0x03 - EIP input assembly test
        DCI_EIP_TEST_INPUT_ASM_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        100U,
        true,
        CIP_ASM_TYPE_LIST_HEAD,
        DCI_EIP_TEST_INPUT_ASM_INST_0x64_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        DCI_EIP_TEST_INPUT_ASM_INST_0x64_ASM_DCID_LIST,
        DCI_EIP_TEST_INPUT_ASM_INST_0x64_ASM_DCID_LIST_LEN
    },
    {        // C: 0x04, I: 0x78, A: 0x03 - EIP output assembly test
        DCI_EIP_TEST_OUTPUT_ASM_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        120U,
        false,
        CIP_ASM_TYPE_LIST_HEAD,
        DCI_EIP_TEST_OUTPUT_ASM_INST_0x78_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        DCI_EIP_TEST_OUTPUT_ASM_INST_0x78_ASM_DCID_LIST,
        DCI_EIP_TEST_OUTPUT_ASM_INST_0x78_ASM_DCID_LIST_LEN
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p Assembly (Instance 150) member 1
        DCI_DYN_IN_ASM_MEMBER_1_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_IN_ASM_MEMBER_1_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p Assembly (Instance 150) member 2
        DCI_DYN_IN_ASM_MEMBER_2_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_IN_ASM_MEMBER_2_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p Assembly (Instance 150) member 3
        DCI_DYN_IN_ASM_MEMBER_3_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_IN_ASM_MEMBER_3_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p Assembly (Instance 150) member 4
        DCI_DYN_IN_ASM_MEMBER_4_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_IN_ASM_MEMBER_4_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p Assembly (Instance 150) member 5
        DCI_DYN_IN_ASM_MEMBER_5_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_IN_ASM_MEMBER_5_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p Assembly (Instance 150) member 6
        DCI_DYN_IN_ASM_MEMBER_6_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_IN_ASM_MEMBER_6_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p Assembly (Instance 150) member 7
        DCI_DYN_IN_ASM_MEMBER_7_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_IN_ASM_MEMBER_7_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p Assembly (Instance 150) member 8
        DCI_DYN_IN_ASM_MEMBER_8_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_IN_ASM_MEMBER_8_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x96, A: 0x03 - Dynamic i/p assembly (Instance 150)
        DCI_DYN_IN_ASM_INTERFACE_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        150U,
        true,
        CIP_ASM_TYPE_LIST_HEAD,
        DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_DCID_LIST,
        DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_DCID_LIST_LEN
    },
    {        // C: 0x04, I: 0x97, A: 0x03 - Dynamic o/p Assembly (Instance 150) member 1
        DCI_DYN_OUT_ASM_MEMBER_1_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        151U,
        false,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_OUT_ASM_MEMBER_1_INST_0x97_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x97, A: 0x03 - Dynamic o/p Assembly (Instance 150) member 2
        DCI_DYN_OUT_ASM_MEMBER_2_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        151U,
        false,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_OUT_ASM_MEMBER_2_INST_0x97_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x97, A: 0x03 - Dynamic o/p Assembly (Instance 150) member 3
        DCI_DYN_OUT_ASM_MEMBER_3_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        151U,
        false,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_OUT_ASM_MEMBER_3_INST_0x97_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x97, A: 0x03 - Dynamic o/p Assembly (Instance 150) member 4
        DCI_DYN_OUT_ASM_MEMBER_4_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        151U,
        false,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_DYN_OUT_ASM_MEMBER_4_INST_0x97_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
    {        // C: 0x04, I: 0x97, A: 0x03 - Dynamic o/p assembly
        DCI_DYN_OUT_ASM_INTERFACE_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        151U,
        false,
        CIP_ASM_TYPE_LIST_HEAD,
        DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_DCID_LIST,
        DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_DCID_LIST_LEN
    },
    {        // C: 0x04, I: 0x78, A: 0x03 - EIP Test Reg 1
        DCI_EIP_TEST1_DCID,
        0U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.
        120U,
        false,
        CIP_ASM_TYPE_LIST_MEMBER,
        DCI_EIP_TEST1_INST_0x78_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.
        reinterpret_cast<DCI_ID_TD*>(NULL),
        static_cast<uint8_t>(NULL)
    },
};


//*******************************************************
//******     The Target Definition Structure.
//*******************************************************

const EIP_TARGET_INFO_ST_TD eip_dci_data_target_info =
{
    EIP_DCI_DATA_DEVICE_TYPE,   //uint16_t identity_dev_type;

    EIP_DCI_DATA_TOTAL_CLASSES, //uint16_t total_items;
    eip_dci_data_cip_class_list,//const DCI_CIP_TO_DCID_LKUP_ST_TD* cip_to_dcid

    eip_dci_data_msg_rtr_attrib1,//uint16_t const* msg_rtr_attrib1

    EIP_DCI_DATA_TOTAL_ASM_ITEMS,//uint16_t total_assemblies;
    4U,                   //uint16_t asm_max_data_len;
    eip_dci_data_dcid_to_asm     //CIP_ASM_STRUCT_TD const* asm_list;
};


