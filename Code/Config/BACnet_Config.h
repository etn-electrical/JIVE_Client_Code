/**
 **********************************************************************************************
 * @file            BACnet_Config.h  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to BACNET
 * component.
 *
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef     BACNET_CONFIG_H
 #define     BACNET_CONFIG_H
#include "Etn_Types.h"


static const uint16_t BACNET_IP_DEFAULT_PORT = 47808U;


static const uint8_t npsmall = 30U;							// number of small buffers
static const uint8_t npbig = 10U;							// number of big buffers


#define useMSTP         1
#define useBIP          1
// #define useEth			1
#define portDefault portMSTP			// change to whatever default you want

#define OBJECT_NAME_WRITE_ENABLE    1

static const uint8_t DEVICE_NAME_MAX_LEN = 21U;

/*
    Enabling define will allow to read any parameter using Modbus ID.
    Write Modbus parameter id in DCI_AV_ANY_PARAM_NUM_DCID
    and then read/write operation on DCI_AV_ANY_PARAM_VAL_DCID will be
    performed on that modbus id parameter.
 */

// #define MODBUS_PARAM_READ_FEATURE_ENABLE

// #define SABUS		1

#endif
