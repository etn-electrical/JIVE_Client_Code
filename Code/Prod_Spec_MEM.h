/**
 *****************************************************************************************
 * @file	: Prod_Spec_MEM.h
 *
 * @brief
 * @details : This file contains declaration and Macros related to Memory
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROD_SPEC_MEM_H
#define PROD_SPEC_MEM_H

#include "NV_Ctrl.h"

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */

/**
 **********************************************************************************************
 * @brief                     Product Specific RAM Initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Target_RAM_Init( void );

/**
 **********************************************************************************************
 * @brief                     Product Specific NV init
 * @param[in] void            none
 * @return[out] void	      none
 * @n
 **********************************************************************************************
 */

void PROD_SPEC_Target_NV_Init( void );

/**
 **********************************************************************************************
 * @brief Get nv control handle of memory space where scratch space defined
 * @param[out] nv_ctrl : Pointer to scratch space NV control.
 * @return None
 **********************************************************************************************
 */
void PROD_SPEC_MEM_Get_Scratch_NV_Handle( NV_Ctrl** nv_ctrl );

/**
 **********************************************************************************************
 * @brief Get nv control handle of memory space where App space defined
 * @param[out] nv_ctrl : Pointer to app space NV control.
 * @return None
 **********************************************************************************************
 */
void PROD_SPEC_MEM_Get_APP_NV_Handle( NV_Ctrl** nv_ctrl );

/**
 **********************************************************************************************
 * @brief                     Product Specific app space area init
 * @param[in] void            none
 * @return[out] void	      none
 * @n
 **********************************************************************************************
 */

void PROD_SPEC_MEM_APP_Space_Init( void );

/**
 **********************************************************************************************
 * @brief Get nv control handle of memory space of the certificates
 * @param[out] nv_ctrl : Pointer to certificate data storage space NV control.
 * @return None
 **********************************************************************************************
 */

void PROD_SPEC_MEM_Dev_Cert_NV_Handle( NV_Ctrl** nv_ctrl );

/**
 **********************************************************************************************
 * @brief                       Product Specific Scratch space area init
 * @param[in] void              none
 * @return[out] void		    none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_MEM_Scratch_Space_Init( void );

/**
 **********************************************************************************************
 * @brief                       Product Specific web image space nv area init
 * @param[in] void              none
 * @return[out] void		    none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_MEM_WEB_Space_Init( void );

/**
 **********************************************************************************************
 * @brief Get nv control handle of memory space of the web image
 * @param[out] nv_ctrl : Pointer to web image storage space NV control.
 * @return None
 **********************************************************************************************
 */

void PROD_SPEC_MEM_Get_WEB_NV_Handle( NV_Ctrl** nv_ctrl );

/**
 **********************************************************************************************
 * @brief                     Product Specific Device certificate storage area initialization
 * @param[in] void            none
 * @return[out] void       	  none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_MEM_Dev_Cert_Init( void );

/**
 **********************************************************************************************
 * @brief                     Product Specific NV Crypto callback to notify error events
 * @param[in] param           Callback parameter
 * @param[in] result          Error type
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_NV_Crypto_Callback( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t result );

/**
 **********************************************************************************************
 * @brief                     Get the shared memory space information
 * @param[in] void            none
 * @return[out] bool_t        Returns flag to indicate memory is shared or not
 * @n
 **********************************************************************************************
 */
bool_t PROD_SPEC_MEM_Is_Scratch_Web_Space_Shared( void );


#endif
