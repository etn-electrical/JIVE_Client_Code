/*
*****************************************************************************************
*       $Workfile:$
*
*       $Author:$
*       $Date:$
*       Copyright © 2011, Eaton Corporation. All Rights Reserved.
*       Creator: Ting Yan
*
*       Description:
*
*
*       Code Inspection Date: 
*
*       $Header:$
*****************************************************************************************
*/
#ifndef REST_DCI_DATA_H
  #define REST_DCI_DATA_H

#include "DCI.h"
#include "Httpd_Rest_DCI.h"

//*******************************************************
//******     RESTful structure and size follow.
//*******************************************************

extern const REST_TARGET_INFO_ST_TD rest_dci_data_target_info;

static const uint8_t USER_ROLE_ADMIN = 99U;
extern const AUTH_STRUCT_TD auth_struct[];
extern const DCI_USER_MGMT_DB_TD USER_MGMT_USER_LIST[];
extern const uint8_t NUM_OF_AUTH_LEVELS;
extern const uint8_t MAX_NUM_OF_AUTH_USERS;
static const uint16_t REST_DCI_MAX_ARRAY_COUNT = 28U;
static const uint16_t REST_DCI_MAX_LENGTH = 170U;


#endif
