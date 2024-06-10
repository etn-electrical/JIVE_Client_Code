/**
 **********************************************************************************************
 * @file            SNTP-ESP32 configuration file
 *
 * @brief           This file contains SNTP ESP32 configuration.
 *
 * @details
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef SNTP_CONFIG_ESP32_H
#define SNTP_CONFIG_ESP32_H

#include "Sntp_If_C_Connector.h"

#define SNTP_DOMAIN_NAME_SUPPORT				///< To Enable SNTP DNS functionality

#define LWIP_SNTP               1
#define SNTP_POLLING_TIME       1000
#define NOT_CONNECTED           SNTP_SYNC_STATUS_RESET
#define CONNECTED               SNTP_SYNC_STATUS_COMPLETED

/** SNTP receive timeout - in milliseconds
 * Also used as retry timeout - this shouldn't be too low.
 * Default is 15 seconds. Must not be below 15 seconds by specification (i.e. 15000)
 */
#define SNTP_RECV_TIMEOUT           SNTP_Get_Sntp_Retry_Time()

/** SNTP update delay - in milliseconds
 * Default is 1 hour. Must not be below 60 seconds by specification (i.e. 60000)
 */
#define SNTP_UPDATE_DELAY           SNTP_Get_Sntp_Update_Time()

#endif	// #ifndef SNTP_CONFIG_ESP32_H
