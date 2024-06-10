/**
 **********************************************************************************************
 * @file            ESP32 Wifi initialization header file
 * @brief           This file contains ESP32 Ethernet initialization function decleration
 * @details			Functions in this file are used to initialize ESP32 Ethernet and PHY
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef PROD_SPEC_WIFI_H
#define PROD_SPEC_WIFI_H

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */

/**
 * @brief Initializes Wifi Interface.
 * @param : None
 */
void PROD_SPEC_WIFI_Init( void );

/**
 * @brief Initializes Wifi Station Interface.
 * @param : None
 */
void WIFI_Station_Init( void );

/**
 * @brief Initializes Wifi Access Point Interface.
 * @param : None
 */
void WIFI_Access_Point_Init( void );

#endif	// #ifndef PROD_SPEC_WIFI_H
