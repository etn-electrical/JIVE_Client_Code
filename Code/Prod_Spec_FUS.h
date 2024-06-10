/**
 **********************************************************************************************
 * @file            Prod_Spec_FUS.h  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product Fw Upgrade.
 *
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef PROD_SPEC_FUS_H
	#define PROD_SPEC_FUS_H
/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI_Owner.h"
#include "FUS_Config.h"
#include "NV_Ctrl.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

typedef struct
{
	uint8_t* last_user;
	uint8_t user_len;
	uint32_t last_time;
} image_upgrade_info_t;

typedef struct fw_upgrade_dcid_s
{
	DCI_Owner* version;
	DCI_Owner* last_user;
	DCI_Owner* last_time;
} fw_upgrade_dcid;

typedef enum web_firm_upgrade_status_t
{
	WEB_FW_UPGRADE_IDLE,
	WEB_FW_UPGRADE_IN_PROGRESS,
	WEB_FW_VALIDATION_COMPLETED
} web_firm_upgrade_status_t;

/*
 *****************************************************************************************
 * @ Initialize the respective web page .
 * @ param[in] ui_type - Maintainance page or webui page.
 * @ param[in] web_ui_nv_ctrl - Need to pass web_ui_nv_ctrl handle to get web area memeory
 *	information.
 * @ return none
 *****************************************************************************************
 */
void Assign_Web_Pages( uint8_t ui_type, NV_Ctrl* web_ui_nv_ctrl );

/*
 *****************************************************************************************
 * @ Set App firmware upgrade state.
 * @return status
 *****************************************************************************************
 */
bool Set_App_Fw_Upgrade_State( uint8_t state );

/*
 *****************************************************************************************
 * @ Get App firmware upgrade state.
 * @return status
 *****************************************************************************************
 */
bool Get_App_Fw_Upgrade_State( uint8_t* state );

/*
 *****************************************************************************************
 * @ Initialize the soft reboot of the device .
 * @ return status
 *****************************************************************************************
 */
int32_t Fw_Reboot( void );

/*
 *****************************************************************************************
 * @ Get the firmware upgrade enabled or not .
 * @return status of upgrade enable or not
 *****************************************************************************************
 */
bool Is_Fw_Upgrade_Enabled( void );

/*
 *****************************************************************************************
 * @brief This function is used to get NV handle of a particular image. Currently it is
 * used by IOT_FUS to get image NV handle
 *
 * @param processor_index:  Processor index
 * @param image_index:      Image index
 * @param image_nv_ctrl:    Image NV handle
 * @return none
 *****************************************************************************************
 */
void Prod_Spec_FUS_Get_Image_NV_Handle( uint8_t processor_index, uint8_t image_index, NV_Ctrl** image_nv_ctrl );

/*
 *****************************************************************************************
 * @brief This function checks whether compatibility number provided in firmware
 * upgrade request is valid or not. Currently, this function is used by IOT_FUS
 * to pass on compatibility number(received from cloud) to product code for validation.
 *
 * @param compat_number_from_interface: Compatibility number received in firmware upgrade
 * request over particular interface
 * @return true if compatible
 *****************************************************************************************
 */
bool Prod_Spec_FUS_Is_Compatible( unsigned long compat_number_from_interface );


/*
 *****************************************************************************************
 * @brief Get nv control handle of memory space where scratch space defined. Currently,
 * this function is used by IOT_FUS to get scratch NV handle(and then erase scratch)
 * @param[out] nv_ctrl : Pointer to scratch space NV control.
 * @return               None
 *****************************************************************************************
 */
void PROD_SPEC_FUS_Get_Scratch_NV_Handle( NV_Ctrl** nv_ctrl );

/*
 *****************************************************************************************
 * @brief This function checks whether image is app image or not. Currently, this function
 * is used by IOT_FUS to identify whether firmware upgrade is for app image. (If that is
 * the case then firmware upgrade success status is sent to the cloud before reboot)
 * @param processor_index:  Processor index
 * @param image_index:      Image index
 * @return                  true if the image is app image
 *****************************************************************************************
 */
bool Prod_Spec_FUS_Is_App_Image( uint8_t processor_index, uint8_t image_index );

/**
 * @brief Initializes FUS Interface.
 * @param : None
 */
void PROD_SPEC_FUS_Init( void );

/**
 * @brief       Function to get data from DCI variables
 * @details     This function will get the DCID value and fill the same in data pointer
 * @param[in]   dcid : DCID number
 * @param[in]   data : Pointer to data buffer
 * @return      Success or failure of requested operation.
 */
DCI_ERROR_TD Get_DCID_Value( DCI_ID_TD dcid, uint8_t* data );

/*
 *****************************************************************************************
 * @brief This function is to update OTA data partition.
 * @ return bool : operation status of the function
 *****************************************************************************************
 */
bool Update_Ota_Data( void );

/*
 *****************************************************************************************
 * @brief	            Get image start address required for firmware download.
 * @param[in]           Processor_index used for component id used to get start address
 * @param[in]           Image_index to decide its an app image or web image to get start address
 * @return				Image start address
 *****************************************************************************************
 */
uint32_t Get_Image_Start_Address( uint8_t processor_index, uint8_t image_index );

#endif
