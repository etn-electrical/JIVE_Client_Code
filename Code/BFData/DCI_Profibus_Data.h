/**
*****************************************************************************************
*   @file
*
*   @brief
*
*   @details
*
*   @version
*   C++ Style Guide Version 1.0
*
*   @copyright 2018 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#ifndef DCI_PROFIBUS_DATA_H
  #define DCI_PROFIBUS_DATA_H

#include "DCI.h"

//*******************************************************
//******     Profibus structure and size follow.
//*******************************************************

#define DCI_PROFIBUS_DATA_TOTAL_MODULES        21U

typedef uint16_t         PROFI_ID_TD;
typedef struct DCI_PROFIBUS_DATA_LKUP_ST_TD
{
    PROFI_ID_TD profibus_id;
    DCI_ID_TD dcid;
} DCI_PROFIBUS_DATA_LKUP_ST_TD;
extern const DCI_PROFIBUS_DATA_LKUP_ST_TD dci_profibus_lkup[DCI_PROFIBUS_DATA_TOTAL_MODULES];


typedef struct DCI_ID_DATA_LKUP_ST_TD
{
    DCI_ID_TD dci_id;
    PROFI_ID_TD profi_id;
} DCI_ID_DATA_LKUP_ST_TD;
extern const DCI_ID_DATA_LKUP_ST_TD dci_id_lkup[DCI_PROFIBUS_DATA_TOTAL_MODULES];


//*******************************************************
//******     Profibus Modules List
//*******************************************************


#define DCI_IP_BIT_NOT_CONNECTED_PROFIBUS_MOD            0U
#define DCI_IP_WORD_NOT_CONNECTED_PROFIBUS_MOD            1U
#define DCI_IP_DWORD_NOT_CONNECTED_PROFIBUS_MOD            2U
#define DCI_STATUS_WARNING_BITS_PROFIBUS_MOD            3U
#define DCI_PRODUCT_ACTIVE_FAULT_PROFIBUS_MOD            4U
#define DCI_PROFIBUS_PROD_MOD_DATA_PROFIBUS_MOD            5U
#define DCI_DEVICE_PARAMETERIZATION_ENABLE_PROFIBUS_MOD            6U
#define DCI_SAMPLE_APP_PARAMETER_PROFIBUS_MOD            7U
#define DCI_SAMPLE_APP_INPUT_BIT_PROFIBUS_MOD            8U
#define DCI_SAMPLE_APP_INPUT_WORD_PROFIBUS_MOD            9U
#define DCI_SAMPLE_APP_INPUT_DWORD_PROFIBUS_MOD            10U
#define DCI_SAMPLE_APP_OUTPUT_BIT_PROFIBUS_MOD            11U
#define DCI_SAMPLE_APP_OUTPUT_WORD_PROFIBUS_MOD            12U
#define DCI_SAMPLE_APP_OUTPUT_DWORD_PROFIBUS_MOD            13U
#define DCI_SAMPLE_APP_INPUT_WORD_SIMPLE_PROFIBUS_MOD            14U
#define DCI_SAMPLE_ACYCLIC_PARAMETER_1_PROFIBUS_MOD            15U
#define DCI_SAMPLE_ACYCLIC_PARAMETER_2_PROFIBUS_MOD            16U
#define DCI_SAMPLE_ACYCLIC_PARAMETER_3_PROFIBUS_MOD            17U
#define DCI_SAMPLE_ACYCLIC_PARAMETER_4_PROFIBUS_MOD            18U
#define DCI_SAMPLE_ACYCLIC_PARAMETER_5_PROFIBUS_MOD            19U
#define DCI_SAMPLE_ACYCLIC_PARAMETER_6_PROFIBUS_MOD            20U


#endif
