/**
 *****************************************************************************************
 * @file User_Management_Json.h
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __USER_MANAGEMENT_JSON_H__
#define __USER_MANAGEMENT_JSON_H__

#include "Includes.h"
#include <stdint.h>
#include "httpd_common.h"
#include "REST_DCI_Data.h"
#include "DCI_Defines.h"
#include "httpd_rest_dci.h"
#include "Base64_Rest.h"
#include "server.h"
#include "REST_Json.h"
#include "httpd_xml.h"
#include "Yjson.h"
#include "DateUtil.h"
#include "TimeUtil.h"

typedef void (* User_Audit_Function_Decl_1 ) ( uint8_t event_code );
typedef void (* User_Audit_Function_Decl_2 ) ( uint8_t* log_param );

void User_Audit_Log_Assign_Callback_Json( User_Audit_Function_Decl_1 user_audit_log_cb,
										  User_Audit_Function_Decl_2 user_audit_param_cb );

/**
 * @brief This function is to update User Audit parameters with latest values.
 * @details This function is to update User Audit parameters with latest values.
 * @param[in] parser : Pointer to the hams parser.
 * @return None.
 */
void User_Audit_Log_Params_Update_Json( hams_parser* parser );

/**
 * @brief This function is to parse the user management URI.
 * @details This function is to to parse the user management when /rs/users is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data: Pointer to the data.
 * @param[in] length: Length of data
 * @return None.
 */
uint32_t Parse_UserMgmt_Resources_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

#endif
