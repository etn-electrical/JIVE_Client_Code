/**
 *****************************************************************************************
 *  @file
 *
 *	@details REST-FUS interface file help to get the execute REST operation.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __REST_FUS_INTERFACE_H__
#define __REST_FUS_INTERFACE_H__


#include <stdint.h>
#include "uC_Code_Range.h"
#include "NV_Ctrl.h"
#include "User_Session.h"
#include "Fw_Store.h"
#include "Prod_Spec_FUS.h"

typedef void (* Fw_Upgrade_User_Function_Decl_2) ( uint8_t image_id, uint8_t user_id );
typedef void (* Fw_Upgrade_User_Function_Decl_1) ( uint8_t image_id, image_upgrade_info_t* image_upgrade_info );
typedef void (* Fw_Upgrade_Audit_Function_Decl_1) ( uint8_t event_code );
typedef void (* Fw_Upgrade_Audit_Function_Decl_2) ( uint8_t event_code, uint8_t* log_param );

void Fw_Upgrade_Audit_Log_Assign_Callback( Fw_Upgrade_Audit_Function_Decl_1 fw_audit_log_cb,
										   Fw_Upgrade_Audit_Function_Decl_2 fw_audit_param_cb );

void User_Firmware_Upgrade_Info( Fw_Upgrade_User_Function_Decl_1 get_user_info_cb,
								 Fw_Upgrade_User_Function_Decl_2 set_user_info_cb );

#define RUN_WEB_UI  1U
#define RUN_MAINTENANCE_UI 0U
#define RUN_DEFAULT_UI 2U

/*
 *****************************************************************************************
 * @Update the user session handler .
 * @return none
 *****************************************************************************************
 */
void Fw_Upgrade_Session_Init_Handler( User_Session* session );

/*
 *****************************************************************************************
 * @ Initialize the Web page .
 * @ param[in] web_ui_nv_ctrl: webui area nv control handle
 * @return none
 *****************************************************************************************
 */
void Web_Page_Init( NV_Ctrl* web_ui_nv_ctrl );

/*
 *****************************************************************************************
 * @ Get the status of the firmware upgrade .
 * @return status of the firmware upgrade.
 *****************************************************************************************
 */
bool Firmware_Upgrade_Inprogress( void );

/*
 *****************************************************************************************
 * @ Delete the active firmware session .
 * @return none.
 *****************************************************************************************
 */
void Exit_firmware_Session( void );

/*
 *****************************************************************************************
 * @ Get the session id from the FUS .
 * @return session id.
 *****************************************************************************************
 */
uint32_t Firmware_Session_Id( void );

/*
 *****************************************************************************************
 * @ Allow the memory space for the firmware upgrade .
 * @return none.
 *****************************************************************************************
 */
bool Allocate_Rest_FUS_Buffer( void );

/*
 *****************************************************************************************
 * @ Free the memory space used for the firmware session .
 * @return none.
 *****************************************************************************************
 */
void Deallocate_Rest_FUS_Buffer( void );

#endif
