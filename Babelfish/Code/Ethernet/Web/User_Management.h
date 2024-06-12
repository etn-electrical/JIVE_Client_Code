/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __USER_MANAGEMENT_H__
#define __USER_MANAGEMENT_H__

#include "Includes.h"
#include <stdint.h>
#include "httpd_common.h"
#include "REST_DCI_Data.h"
#include "DCI_Defines.h"
#include "httpd_rest_dci.h"
#include "Base64_Rest.h"
#include "server.h"
#include "REST.h"
#include "httpd_xml.h"
#include "yxml.h"
#include "DateUtil.h"
#include "TimeUtil.h"

typedef void (* User_Audit_Function_Decl_1 ) ( uint8_t event_code );
typedef void (* User_Audit_Function_Decl_2 ) ( uint8_t* log_param );

void User_Audit_Log_Assign_Callback( User_Audit_Function_Decl_1 user_audit_log_cb,
									 User_Audit_Function_Decl_2 user_audit_param_cb );

/**
 *  @brief  function to update User Audit parameters with latest values
 */
void User_Audit_Log_Params_Update( hams_parser* parser );

#endif
