/**
 **********************************************************************************************
 * @file            ESP32 Ethernet initialization header file
 * @brief           This file contains ESP32 Ethernet initialization function decleration
 * @details			Functions in this file are used to initialize ESP32 Ethernet and PHY
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef PROD_SPEC_ETHERNET_H
#define PROD_SPEC_ETHERNET_H

/**
 * @brief: product specific Ethernet/EMAC initialization
 * 		   This function initializes ESP32 Ethernet using Ethernet TCP/IP stack provided by
 * 		   Espressif
 *
 *
 *
 * @param[in] void
 * @param[out] void
 */
void PROD_SPEC_ETH_Init( void );

#endif	// #ifndef PROD_SPEC_ETHERNET_H
