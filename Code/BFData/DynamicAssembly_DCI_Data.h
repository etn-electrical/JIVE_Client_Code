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
*   @copyright 2016 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#ifndef DYNAMICASSEMBLY_DCI_DATA_H
  #define DYNAMICASSEMBLY_DCI_DATA_H

#include "DCI.h"
#include "DCI_Owner.h"


//*******************************************************
//******Dynamic Assembly structure and size follow.
//*******************************************************

typedef uint16_t DYNAMIC_ASSEMBLY_PID_TD;
typedef uint16_t dynamic_assembly_pid_t;


typedef struct DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD
{
     DYNAMIC_ASSEMBLY_PID_TD assembly_id;
     DCI_ID_TD dcid;
} DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD;


typedef struct DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD
{
     DCI_ID_TD dcid;
     DYNAMIC_ASSEMBLY_PID_TD assembly_id;
} DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD;


typedef struct D_IN_ASM_TARGET_INFO_ST_TD
{
     DYNAMIC_ASSEMBLY_PID_TD total_d_in_assemblies;
     const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* asmin_to_dcid;
     DCI_ID_TD total_dcids;
     const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD* dcid_to_asmin;
} D_IN_ASM_TARGET_INFO_ST_TD;


typedef struct DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD
{
     DYNAMIC_ASSEMBLY_PID_TD assembly_id;
     DCI_ID_TD dcid;
} DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD;


typedef struct DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD
{
     DCI_ID_TD dcid;
     DYNAMIC_ASSEMBLY_PID_TD assembly_id;
} DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD;


typedef struct D_OUT_ASM_TARGET_INFO_ST_TD
{
     DYNAMIC_ASSEMBLY_PID_TD total_d_in_assemblies;
     const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* asmout_to_dcid;
     DCI_ID_TD total_dcids;
     const DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD* dcid_to_asmin;
} D_OUT_ASM_TARGET_INFO_ST_TD;


extern const D_IN_ASM_TARGET_INFO_ST_TD d_in_asm_dci_data_target_info;

static const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* const LOOKUP_D_IN_ASM_TO_DCI_NOT_FOUND=NULL;
static const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD* const LOOKUP_DCI_TO_D_IN_ASM_NOT_FOUND=NULL;


extern const D_OUT_ASM_TARGET_INFO_ST_TD d_out_asm_dci_data_target_info;

static const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* const LOOKUP_D_OUT_ASM_TO_DCI_NOT_FOUND=NULL;
static const DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD* const LOOKUP_DCI_TO_D_OUT_ASM_NOT_FOUND=NULL;

static const uint8_t D_IN_ASM_MAX =   8U;
static const uint8_t D_OUT_ASM_MAX =   4U;
void init_dyn_input_asm_dcids (DCI_ID_TD * dyn_in_asm_dcid, DCI_Owner * m_d_in_Asm_Selection[]);
void init_dyn_output_asm_dcids(DCI_ID_TD * dyn_out_asm_dcid, DCI_Owner * m_d_out_Asm_Selection[]);


#endif
