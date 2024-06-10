/**
 *****************************************************************************************
 * @file	: Prod_Spec_EIP.h
 *
 * @brief
 * @details : This file contains declaration and Macros related to EIP
 *
 *
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROD_SPEC_EIP_H

#define PROD_SPEC_EIP_H
#include "Includes.h"
#include "DCI_Defines.h"
#include "Output_Pos.h"

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */
/**
 **********************************************************************************************
 * @brief                    Initialize EIP
 * @param[in] none           none
 * @return[out] none         none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_EIP_Init( void );

/**
 **********************************************************************************************
 * @brief                    Initialize EIP stack. Start and then restart EIP stack
 * @param[in] none           none
 * @return[out] none         none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_EIP_Stack_Init( void );

/**
 **********************************************************************************************
 * @brief                    Executes fault reset call back
 * @param[in] handle         not used in the function
 * @param[in] access_struct  Used to execute required command
 * @return[out] none         none
 * @n
 **********************************************************************************************
 */
DCI_CB_RET_TD PROD_SPEC_Fault_Reset_CB( DCI_CBACK_PARAM_TD handle, DCI_ACCESS_ST_TD* access_struct );

/**
 **********************************************************************************************
 * @brief                    Returns mapped DCIID
 * @param[in] index          DCIID index
 * @return[out] DCI_ID_TD    Returns mapped DCID
 * @n
 **********************************************************************************************
 */
DCI_ID_TD Return_Mapped_DCID( uint8_t index );

#endif
