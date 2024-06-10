/**
*****************************************************************************************
*   @file
*   @details See header file for module overview.
*   @copyright 2016 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#include "Includes.h"
#include "DynamicAssembly_DCI_Data.h"



//*******************************************************
//******  Dynamic input Assembly List
//*******************************************************

static const dynamic_assembly_pid_t DCI_DYN_IN_ASM_TERMINATOR_IN_DASM_ID = 0U;
static const dynamic_assembly_pid_t DCI_CIP_VENDOR_ID_IN_DASM_ID = 1U;
static const dynamic_assembly_pid_t DCI_CIP_DEVICE_TYPE_IN_DASM_ID = 2U;
static const dynamic_assembly_pid_t DCI_PRODUCT_CODE_IN_DASM_ID = 3U;
static const dynamic_assembly_pid_t DCI_APP_FIRM_VER_IN_DASM_ID = 4U;
static const dynamic_assembly_pid_t DCI_CIP_DEVICE_IDENT_DEV_STATUS_IN_DASM_ID = 5U;
static const dynamic_assembly_pid_t DCI_PRODUCT_SERIAL_NUM_IN_DASM_ID = 6U;
static const dynamic_assembly_pid_t DCI_PRODUCT_NAME_IN_DASM_ID = 7U;
static const dynamic_assembly_pid_t DCI_SIMPLE_APP_STATUS_WORD_IN_DASM_ID = 8U;
static const dynamic_assembly_pid_t DCI_SIMPLE_APP_CONTROL_WORD_IN_DASM_ID = 9U;
static const dynamic_assembly_pid_t DCI_MODBUS_TCP_COMM_TIMEOUT_IN_DASM_ID = 10U;
static const dynamic_assembly_pid_t DCI_MODBUS_SERIAL_COMM_TIMEOUT_IN_DASM_ID = 11U;
static const dynamic_assembly_pid_t DCI_RESET_COUNTER_IN_DASM_ID = 12U;
static const dynamic_assembly_pid_t DCI_PHY1_LINK_SPEED_ACTUAL_IN_DASM_ID = 13U;
static const dynamic_assembly_pid_t DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_IN_DASM_ID = 14U;
static const dynamic_assembly_pid_t DCI_IDLE_LOOP_TIME_AVE_IN_DASM_ID = 15U;
static const dynamic_assembly_pid_t DCI_IDLE_LOOP_TIME_MAX_IN_DASM_ID = 16U;
static const dynamic_assembly_pid_t DCI_IDLE_LOOP_TIME_MIN_IN_DASM_ID = 17U;

//*******************************************************
//****** The Dynamic i/p Assembly Counts.
//*******************************************************
static const dynamic_assembly_pid_t DYNAMIC_IN_ASSEMBLY_DATA_TOTAL_IDS = 18U;
static DCI_ID_TD DYNAMIC_IN_ASSEMBLY_DCI_DATA_TOTAL_DCIDS = 18U;


//*******************************************************
//****** Dynamic output Assembly List
//*******************************************************

static const dynamic_assembly_pid_t DCI_DYN_OUT_ASM_TERMINATOR_OUT_DASM_ID = 0U;
static const dynamic_assembly_pid_t DCI_EIP_TEST1_OUT_DASM_ID = 1U;
static const dynamic_assembly_pid_t DCI_EIP_TEST2_OUT_DASM_ID = 2U;
static const dynamic_assembly_pid_t DCI_EIP_TEST3_OUT_DASM_ID = 3U;
static const dynamic_assembly_pid_t DCI_EIP_TEST4_OUT_DASM_ID = 4U;

//*******************************************************
//****** The Dynamic o/p Assembly Counts.
//*******************************************************
static const dynamic_assembly_pid_t DYNAMIC_OUT_ASSEMBLY_DATA_TOTAL_IDS = 5U;
static DCI_ID_TD DYNAMIC_OUT_ASSEMBLY_DCI_DATA_TOTAL_DCIDS = 5U;


//*******************************************************
//****    The Dynamic i/p Assembly Cross Reference Table.
//*******************************************************

static const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD dynamic_in_assem_dci_data_id_to_dcid[DYNAMIC_IN_ASSEMBLY_DATA_TOTAL_IDS] = 
{
    { 
        DCI_DYN_IN_ASM_TERMINATOR_IN_DASM_ID,
        DCI_DYN_IN_ASM_TERMINATOR_DCID
    }, 
    { 
        DCI_CIP_VENDOR_ID_IN_DASM_ID,
        DCI_CIP_VENDOR_ID_DCID
    }, 
    { 
        DCI_CIP_DEVICE_TYPE_IN_DASM_ID,
        DCI_CIP_DEVICE_TYPE_DCID
    }, 
    { 
        DCI_PRODUCT_CODE_IN_DASM_ID,
        DCI_PRODUCT_CODE_DCID
    }, 
    { 
    	DCI_APP_FIRM_VER_IN_DASM_ID,
		DCI_APP_FIRM_VER_DCID
    }, 
    { 
        DCI_CIP_DEVICE_IDENT_DEV_STATUS_IN_DASM_ID,
        DCI_CIP_DEVICE_IDENT_DEV_STATUS_DCID
    }, 
    { 
        DCI_PRODUCT_SERIAL_NUM_IN_DASM_ID,
        DCI_PRODUCT_SERIAL_NUM_DCID
    }, 
    { 
        DCI_PRODUCT_NAME_IN_DASM_ID,
        DCI_PRODUCT_NAME_DCID
    }, 
    { 
        DCI_SIMPLE_APP_STATUS_WORD_IN_DASM_ID,
        DCI_SIMPLE_APP_STATUS_WORD_DCID
    }, 
    { 
        DCI_SIMPLE_APP_CONTROL_WORD_IN_DASM_ID,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID
    }, 
    { 
        DCI_MODBUS_TCP_COMM_TIMEOUT_IN_DASM_ID,
        DCI_MODBUS_TCP_COMM_TIMEOUT_DCID
    }, 
    { 
        DCI_MODBUS_SERIAL_COMM_TIMEOUT_IN_DASM_ID,
        DCI_MODBUS_SERIAL_COMM_TIMEOUT_DCID
    }, 
    { 
        DCI_RESET_COUNTER_IN_DASM_ID,
        DCI_RESET_COUNTER_DCID
    }, 
    { 
        DCI_PHY1_LINK_SPEED_ACTUAL_IN_DASM_ID,
        DCI_PHY1_LINK_SPEED_ACTUAL_DCID
    }, 
    { 
        DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_IN_DASM_ID,
        DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_DCID
    }, 
    { 
        DCI_IDLE_LOOP_TIME_AVE_IN_DASM_ID,
        DCI_IDLE_LOOP_TIME_AVE_DCID
    }, 
    { 
        DCI_IDLE_LOOP_TIME_MAX_IN_DASM_ID,
        DCI_IDLE_LOOP_TIME_MAX_DCID
    }, 
    { 
        DCI_IDLE_LOOP_TIME_MIN_IN_DASM_ID,
        DCI_IDLE_LOOP_TIME_MIN_DCID
    }, 
};
const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD dynamic_in_asm_dci_data_dcid_to_id[DYNAMIC_IN_ASSEMBLY_DATA_TOTAL_IDS] = 
{
    { 
        DCI_DYN_IN_ASM_TERMINATOR_DCID,
        DCI_DYN_IN_ASM_TERMINATOR_IN_DASM_ID
    }, 
    { 
        DCI_CIP_VENDOR_ID_DCID,
        DCI_CIP_VENDOR_ID_IN_DASM_ID
    }, 
    { 
        DCI_CIP_DEVICE_TYPE_DCID,
        DCI_CIP_DEVICE_TYPE_IN_DASM_ID
    }, 
    { 
        DCI_PRODUCT_CODE_DCID,
        DCI_PRODUCT_CODE_IN_DASM_ID
    }, 
    { 
    	DCI_APP_FIRM_VER_DCID,
		DCI_APP_FIRM_VER_IN_DASM_ID
    }, 
    { 
        DCI_CIP_DEVICE_IDENT_DEV_STATUS_DCID,
        DCI_CIP_DEVICE_IDENT_DEV_STATUS_IN_DASM_ID
    }, 
    { 
        DCI_PRODUCT_SERIAL_NUM_DCID,
        DCI_PRODUCT_SERIAL_NUM_IN_DASM_ID
    }, 
    { 
        DCI_PRODUCT_NAME_DCID,
        DCI_PRODUCT_NAME_IN_DASM_ID
    }, 
    { 
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        DCI_SIMPLE_APP_STATUS_WORD_IN_DASM_ID
    }, 
    { 
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        DCI_SIMPLE_APP_CONTROL_WORD_IN_DASM_ID
    }, 
    { 
        DCI_MODBUS_TCP_COMM_TIMEOUT_DCID,
        DCI_MODBUS_TCP_COMM_TIMEOUT_IN_DASM_ID
    }, 
    { 
        DCI_MODBUS_SERIAL_COMM_TIMEOUT_DCID,
        DCI_MODBUS_SERIAL_COMM_TIMEOUT_IN_DASM_ID
    }, 
    { 
        DCI_RESET_COUNTER_DCID,
        DCI_RESET_COUNTER_IN_DASM_ID
    }, 
    { 
        DCI_PHY1_LINK_SPEED_ACTUAL_DCID,
        DCI_PHY1_LINK_SPEED_ACTUAL_IN_DASM_ID
    }, 
    { 
        DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_DCID,
        DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_IN_DASM_ID
    }, 
    { 
        DCI_IDLE_LOOP_TIME_AVE_DCID,
        DCI_IDLE_LOOP_TIME_AVE_IN_DASM_ID
    }, 
    { 
        DCI_IDLE_LOOP_TIME_MAX_DCID,
        DCI_IDLE_LOOP_TIME_MAX_IN_DASM_ID
    }, 
    { 
        DCI_IDLE_LOOP_TIME_MIN_DCID,
        DCI_IDLE_LOOP_TIME_MIN_IN_DASM_ID
    }, 
};

//*******************************************************
//****    The Dynamic o/p Assembly Cross Reference Table.
//*******************************************************

static const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD dynamic_out_assem_dci_data_id_to_dcid[DYNAMIC_OUT_ASSEMBLY_DATA_TOTAL_IDS] = 
{
    { 
        DCI_DYN_OUT_ASM_TERMINATOR_OUT_DASM_ID,
        DCI_DYN_OUT_ASM_TERMINATOR_DCID
    }, 
    { 
        DCI_EIP_TEST1_OUT_DASM_ID,
        DCI_EIP_TEST1_DCID
    }, 
    { 
        DCI_EIP_TEST2_OUT_DASM_ID,
        DCI_EIP_TEST2_DCID
    }, 
    { 
        DCI_EIP_TEST3_OUT_DASM_ID,
        DCI_EIP_TEST3_DCID
    }, 
    { 
        DCI_EIP_TEST4_OUT_DASM_ID,
        DCI_EIP_TEST4_DCID
    }, 
};
const DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD dynamic_out_asm_dci_data_dcid_to_id[DYNAMIC_OUT_ASSEMBLY_DATA_TOTAL_IDS] = 
{
    { 
        DCI_DYN_OUT_ASM_TERMINATOR_DCID,
        DCI_DYN_OUT_ASM_TERMINATOR_OUT_DASM_ID
    }, 
    { 
        DCI_EIP_TEST1_DCID,
        DCI_EIP_TEST1_OUT_DASM_ID
    }, 
    { 
        DCI_EIP_TEST2_DCID,
        DCI_EIP_TEST2_OUT_DASM_ID
    }, 
    { 
        DCI_EIP_TEST3_DCID,
        DCI_EIP_TEST3_OUT_DASM_ID
    }, 
    { 
        DCI_EIP_TEST4_DCID,
        DCI_EIP_TEST4_OUT_DASM_ID
    }, 
};

//*******************************************************
//** The Dynamic i/p Assembly Target Definition Structure.
//*******************************************************

const D_IN_ASM_TARGET_INFO_ST_TD  d_in_asm_dci_data_target_info = 
{
    DYNAMIC_IN_ASSEMBLY_DATA_TOTAL_IDS,        //DYNAMIC_ASSEMBLY_PID_TD total_d_in_assemblies
    dynamic_in_assem_dci_data_id_to_dcid,      //const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* asmin_to_dcid
    DYNAMIC_IN_ASSEMBLY_DCI_DATA_TOTAL_DCIDS,  //DCI_ID_TD total_dcids
    dynamic_in_asm_dci_data_dcid_to_id       //const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD* dcid_to_asmin
};


//*******************************************************
//** The Dynamic o/p Assembly Target Definition Structure.
//*******************************************************

const D_OUT_ASM_TARGET_INFO_ST_TD  d_out_asm_dci_data_target_info = 
{
    DYNAMIC_OUT_ASSEMBLY_DATA_TOTAL_IDS,        //DYNAMIC_ASSEMBLY_PID_TD total_d_out_assemblies
    dynamic_out_assem_dci_data_id_to_dcid,      //const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* asmout_to_dcid
    DYNAMIC_OUT_ASSEMBLY_DCI_DATA_TOTAL_DCIDS,  //DCI_ID_TD total_dcids
    dynamic_out_asm_dci_data_dcid_to_id       //const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD* dcid_to_asmout
};


//*******************************************************
//** Init Dynamic i/p Assembly IDs.
//*******************************************************

void init_dyn_input_asm_dcids( DCI_ID_TD * dyn_in_asm_dcid, DCI_Owner * m_d_in_Asm_Selection[])
{
    uint8_t temp_uint8 = 0U;

    new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_1_DCID );
    dyn_in_asm_dcid[0] = DCI_DYN_IN_ASM_SELECT_1_DCID;
    new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_2_DCID );
    dyn_in_asm_dcid[1] = DCI_DYN_IN_ASM_SELECT_2_DCID;
    new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_3_DCID );
    dyn_in_asm_dcid[2] = DCI_DYN_IN_ASM_SELECT_3_DCID;
    new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_4_DCID );
    dyn_in_asm_dcid[3] = DCI_DYN_IN_ASM_SELECT_4_DCID;
    new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_5_DCID );
    dyn_in_asm_dcid[4] = DCI_DYN_IN_ASM_SELECT_5_DCID;
    new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_6_DCID );
    dyn_in_asm_dcid[5] = DCI_DYN_IN_ASM_SELECT_6_DCID;
    new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_7_DCID );
    dyn_in_asm_dcid[6] = DCI_DYN_IN_ASM_SELECT_7_DCID;
    new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_8_DCID );
    dyn_in_asm_dcid[7] = DCI_DYN_IN_ASM_SELECT_8_DCID;
    for ( temp_uint8=0U; temp_uint8 < D_IN_ASM_MAX; temp_uint8++)
    {
        m_d_in_Asm_Selection[temp_uint8] = new DCI_Owner( dyn_in_asm_dcid[temp_uint8]);
    }
}


//*******************************************************
//** Init Dynamic o/p Assembly IDs.
//*******************************************************

void init_dyn_output_asm_dcids( DCI_ID_TD * dyn_out_asm_dcid, DCI_Owner * m_d_out_Asm_Selection[])
{
    uint8_t temp_uint8 = 0U;

    new DCI_Owner( DCI_DYN_OUT_ASM_MEMBER_1_DCID );
    dyn_out_asm_dcid[0] = DCI_DYN_OUT_ASM_SELECT_1_DCID;
    new DCI_Owner( DCI_DYN_OUT_ASM_MEMBER_2_DCID );
    dyn_out_asm_dcid[1] = DCI_DYN_OUT_ASM_SELECT_2_DCID;
    new DCI_Owner( DCI_DYN_OUT_ASM_MEMBER_3_DCID );
    dyn_out_asm_dcid[2] = DCI_DYN_OUT_ASM_SELECT_3_DCID;
    new DCI_Owner( DCI_DYN_OUT_ASM_MEMBER_4_DCID );
    dyn_out_asm_dcid[3] = DCI_DYN_OUT_ASM_SELECT_4_DCID;
    for ( temp_uint8=0U; temp_uint8 < D_OUT_ASM_MAX; temp_uint8++)
    {
        m_d_out_Asm_Selection[temp_uint8] = new DCI_Owner( dyn_out_asm_dcid[temp_uint8]);
    }
}


