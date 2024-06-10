/**
*****************************************************************************************
*   @file
*   @details See header file for module overview.
*   @copyright 2018 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#include "Includes.h"
#include "DCI_Profibus_Data.h"



//*******************************************************
//******    The Profibus Parameter Cross Reference Table.
//*******************************************************

const DCI_PROFIBUS_DATA_LKUP_ST_TD dci_profibus_lkup[DCI_PROFIBUS_DATA_TOTAL_MODULES] = 
{
    {        // IP_BIT_NOT_CONNECTED
        DCI_IP_BIT_NOT_CONNECTED_PROFIBUS_MOD,
        DCI_IP_BIT_NOT_CONNECTED_DCID,
    }, 
    {        // IP_WORD_NOT_CONNECTED
        DCI_IP_WORD_NOT_CONNECTED_PROFIBUS_MOD,
        DCI_IP_WORD_NOT_CONNECTED_DCID,
    }, 
    {        // IP_DWORD_NOT_CONNECTED
        DCI_IP_DWORD_NOT_CONNECTED_PROFIBUS_MOD,
        DCI_IP_DWORD_NOT_CONNECTED_DCID,
    }, 
    {        // Warning Status Bits
        DCI_STATUS_WARNING_BITS_PROFIBUS_MOD,
        DCI_STATUS_WARNING_BITS_DCID,
    }, 
    {        // Product Active Fault
        DCI_PRODUCT_ACTIVE_FAULT_PROFIBUS_MOD,
        DCI_PRODUCT_ACTIVE_FAULT_DCID,
    }, 
    {        // Profibus Production Data
        DCI_PROFIBUS_PROD_MOD_DATA_PROFIBUS_MOD,
        DCI_PROFIBUS_PROD_MOD_DATA_DCID,
    }, 
    {        // Parameterization Download Enable
        DCI_DEVICE_PARAMETERIZATION_ENABLE_PROFIBUS_MOD,
        DCI_DEVICE_PARAMETERIZATION_ENABLE_DCID,
    }, 
    {        // Profibus Sample App Parameter
        DCI_SAMPLE_APP_PARAMETER_PROFIBUS_MOD,
        DCI_SAMPLE_APP_PARAMETER_DCID,
    }, 
    {        // Profibus Sample App Input Bit
        DCI_SAMPLE_APP_INPUT_BIT_PROFIBUS_MOD,
        DCI_SAMPLE_APP_INPUT_BIT_DCID,
    }, 
    {        // Profibus Sample App Input word
        DCI_SAMPLE_APP_INPUT_WORD_PROFIBUS_MOD,
        DCI_SAMPLE_APP_INPUT_WORD_DCID,
    }, 
    {        // Profibus Sample App Input Dword
        DCI_SAMPLE_APP_INPUT_DWORD_PROFIBUS_MOD,
        DCI_SAMPLE_APP_INPUT_DWORD_DCID,
    }, 
    {        // Profibus Sample App Output Bit
        DCI_SAMPLE_APP_OUTPUT_BIT_PROFIBUS_MOD,
        DCI_SAMPLE_APP_OUTPUT_BIT_DCID,
    }, 
    {        // Profibus Sample App Output word
        DCI_SAMPLE_APP_OUTPUT_WORD_PROFIBUS_MOD,
        DCI_SAMPLE_APP_OUTPUT_WORD_DCID,
    }, 
    {        // Profibus Sample App Output Dword
        DCI_SAMPLE_APP_OUTPUT_DWORD_PROFIBUS_MOD,
        DCI_SAMPLE_APP_OUTPUT_DWORD_DCID,
    }, 
    {        // Simple Input word
        DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_PROFIBUS_MOD,
        DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_DCID,
    }, 
    {        // Profibus Sample Acyclic Parameter 1 
        DCI_SAMPLE_ACYCLIC_PARAMETER_1_PROFIBUS_MOD,
        DCI_SAMPLE_ACYCLIC_PARAMETER_1_DCID,
    }, 
    {        // Profibus Sample Acyclic Parameter 2 
        DCI_SAMPLE_ACYCLIC_PARAMETER_2_PROFIBUS_MOD,
        DCI_SAMPLE_ACYCLIC_PARAMETER_2_DCID,
    }, 
    {        // Profibus Sample Acyclic  Parameter 3 
        DCI_SAMPLE_ACYCLIC_PARAMETER_3_PROFIBUS_MOD,
        DCI_SAMPLE_ACYCLIC_PARAMETER_3_DCID,
    }, 
    {        // Profibus Sample Acyclic Parameter 4 
        DCI_SAMPLE_ACYCLIC_PARAMETER_4_PROFIBUS_MOD,
        DCI_SAMPLE_ACYCLIC_PARAMETER_4_DCID,
    }, 
    {        // Profibus Sample Acyclic  Parameter 5 
        DCI_SAMPLE_ACYCLIC_PARAMETER_5_PROFIBUS_MOD,
        DCI_SAMPLE_ACYCLIC_PARAMETER_5_DCID,
    }, 
    {        // Profibus Sample Acyclic  Parameter 6 
        DCI_SAMPLE_ACYCLIC_PARAMETER_6_PROFIBUS_MOD,
        DCI_SAMPLE_ACYCLIC_PARAMETER_6_DCID,
    }, 
};


//*******************************************************
//***The DCI to Profibus Parameter Cross Reference Table.
//*******************************************************

const DCI_ID_DATA_LKUP_ST_TD dci_id_lkup[DCI_PROFIBUS_DATA_TOTAL_MODULES] = 
{
    {        // IP_BIT_NOT_CONNECTED
        DCI_IP_BIT_NOT_CONNECTED_DCID,
        DCI_IP_BIT_NOT_CONNECTED_PROFIBUS_MOD
    }, 
    {        // IP_WORD_NOT_CONNECTED
        DCI_IP_WORD_NOT_CONNECTED_DCID,
        DCI_IP_WORD_NOT_CONNECTED_PROFIBUS_MOD
    }, 
    {        // IP_DWORD_NOT_CONNECTED
        DCI_IP_DWORD_NOT_CONNECTED_DCID,
        DCI_IP_DWORD_NOT_CONNECTED_PROFIBUS_MOD
    }, 
    {        // Parameterization Download Enable
        DCI_DEVICE_PARAMETERIZATION_ENABLE_DCID,
        DCI_DEVICE_PARAMETERIZATION_ENABLE_PROFIBUS_MOD
    }, 
    {        // Profibus Production Data
        DCI_PROFIBUS_PROD_MOD_DATA_DCID,
        DCI_PROFIBUS_PROD_MOD_DATA_PROFIBUS_MOD
    }, 
    {        // Warning Status Bits
        DCI_STATUS_WARNING_BITS_DCID,
        DCI_STATUS_WARNING_BITS_PROFIBUS_MOD
    }, 
    {        // Product Active Fault
        DCI_PRODUCT_ACTIVE_FAULT_DCID,
        DCI_PRODUCT_ACTIVE_FAULT_PROFIBUS_MOD
    }, 
    {        // Profibus Sample App Input Bit
        DCI_SAMPLE_APP_INPUT_BIT_DCID,
        DCI_SAMPLE_APP_INPUT_BIT_PROFIBUS_MOD
    }, 
    {        // Profibus Sample App Input word
        DCI_SAMPLE_APP_INPUT_WORD_DCID,
        DCI_SAMPLE_APP_INPUT_WORD_PROFIBUS_MOD
    }, 
    {        // Profibus Sample App Input Dword
        DCI_SAMPLE_APP_INPUT_DWORD_DCID,
        DCI_SAMPLE_APP_INPUT_DWORD_PROFIBUS_MOD
    }, 
    {        // Profibus Sample App Output Bit
        DCI_SAMPLE_APP_OUTPUT_BIT_DCID,
        DCI_SAMPLE_APP_OUTPUT_BIT_PROFIBUS_MOD
    }, 
    {        // Profibus Sample App Output word
        DCI_SAMPLE_APP_OUTPUT_WORD_DCID,
        DCI_SAMPLE_APP_OUTPUT_WORD_PROFIBUS_MOD
    }, 
    {        // Profibus Sample App Output Dword
        DCI_SAMPLE_APP_OUTPUT_DWORD_DCID,
        DCI_SAMPLE_APP_OUTPUT_DWORD_PROFIBUS_MOD
    }, 
    {        // Simple Input word
        DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_DCID,
        DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_PROFIBUS_MOD
    }, 
    {        // Profibus Sample App Parameter
        DCI_SAMPLE_APP_PARAMETER_DCID,
        DCI_SAMPLE_APP_PARAMETER_PROFIBUS_MOD
    }, 
    {        // Profibus Sample Acyclic Parameter 1 
        DCI_SAMPLE_ACYCLIC_PARAMETER_1_DCID,
        DCI_SAMPLE_ACYCLIC_PARAMETER_1_PROFIBUS_MOD
    }, 
    {        // Profibus Sample Acyclic Parameter 2 
        DCI_SAMPLE_ACYCLIC_PARAMETER_2_DCID,
        DCI_SAMPLE_ACYCLIC_PARAMETER_2_PROFIBUS_MOD
    }, 
    {        // Profibus Sample Acyclic  Parameter 3 
        DCI_SAMPLE_ACYCLIC_PARAMETER_3_DCID,
        DCI_SAMPLE_ACYCLIC_PARAMETER_3_PROFIBUS_MOD
    }, 
    {        // Profibus Sample Acyclic Parameter 4 
        DCI_SAMPLE_ACYCLIC_PARAMETER_4_DCID,
        DCI_SAMPLE_ACYCLIC_PARAMETER_4_PROFIBUS_MOD
    }, 
    {        // Profibus Sample Acyclic  Parameter 5 
        DCI_SAMPLE_ACYCLIC_PARAMETER_5_DCID,
        DCI_SAMPLE_ACYCLIC_PARAMETER_5_PROFIBUS_MOD
    }, 
    {        // Profibus Sample Acyclic  Parameter 6 
        DCI_SAMPLE_ACYCLIC_PARAMETER_6_DCID,
        DCI_SAMPLE_ACYCLIC_PARAMETER_6_PROFIBUS_MOD
    }, 
};
