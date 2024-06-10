/**
 *****************************************************************************************
 * @file	: Prod_Spec_Webserver.h
 *
 * @brief
 * @details : this file contains declaration and Macros related to Ethernet initialization
 *
 *
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROD_SPEC_WEB_SERVER_H
#define PROD_SPEC_WEB_SERVER_H

#include "DCI_Defines.h"

void PROD_SPEC_WEBSERVER_Init( void );

/**
 * @brief Callback Function to set device unique password
 * @details This callback function is used to set the Device Unique password and uses DCI_DEVICE_DEFAULT_PWD_DCID
 * Whenever a write is triggered ( For ex. from Modbus or REST ) this callback will be invoked and will set the device
 * default admin password received.
 * Manufacturing lock feature has been added for this callback mechanism i.e. if the manufacturing lock is set, Write
 * operation on this DCID will be blocked. Hence to set the device unique password, one has to unlock the device first
 * and then write the password.
 * Resetting the device will again lock the write operation on this DCID.
 * @param[in] param: parameter
 * @param[in] access_struct: Data structure
 * @return True if successful, False if un-successful.
 */
DCI_CB_RET_TD Prod_Spec_Device_Default_Pwd_CB( DCI_CBACK_PARAM_TD param, DCI_ACCESS_ST_TD* access_struct );

#endif
