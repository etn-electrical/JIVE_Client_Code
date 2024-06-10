/**
*****************************************************************************************
*
*   @file
*
*   @brief
*
*   @details
*
*   @version
*   C++ Style Guide Version 1.0
*
*   @copyright 2020 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#ifndef EIP_DCI_DATA_H
  #define EIP_DCI_DATA_H

#include "DCI.h"
#include "EtherNetIP_DCI.h"

static const uint8_t CIP_TO_DCI_CLASS_BYTE_SIZE =     1U;
static const uint8_t CIP_TO_DCI_INSTANCE_BYTE_SIZE =  1U;
static const uint8_t CIP_TO_DCI_ATTRIBUTE_BYTE_SIZE = 1U;


//*******************************************************
//******     CIP structure and size follow.
//*******************************************************

extern const EIP_TARGET_INFO_ST_TD eip_dci_data_target_info;
extern const DCI_ID_TD DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_DCID_LIST[8];
extern const DCI_ID_TD DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_DCID_LIST[4];
/** Above lines are temporarily hardcoded for Dynamic Assembly.
Hardcode will be removed later on.
'DCI_DYN_IN_ASM_INTERFACE' & 'DCI_DYN_OUT_ASM_INTERFACE' are Headers
for Dynamic Assembly instance 150 & 151.
So Dynamic Assembly name & Length will require to change if in
future these are changed in 'EtherNetIP' tab in Spreadsheet.
*/


#endif
