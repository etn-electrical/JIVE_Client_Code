/**
*****************************************************************************************
*  @file
*  @brief Sample database file for IOT_DCI communication.
*
*  @details
*
*  @copyright 2018 Eaton Corporation. All Rights Reserved.
*
 *****************************************************************************************
*/

#ifndef IOT_DCI_DATA_H
#define IOT_DCI_DATA_H

#include "DCI.h"
#include "DCI_Defines.h"
#include "IoT_DCI.h"

//*******************************************************
//******     IOT structure and size follow.
//*******************************************************

extern const iot_target_info_st_t iot_dci_data_target_info;
static const uint16_t IOT_DCI_DATA_BUFFER_SIZE = 1346U;
static const uint16_t IOT_DCI_DATA_MAX_VAL_LENGTH = 103U;

#endif
