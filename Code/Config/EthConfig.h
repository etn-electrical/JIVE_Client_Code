/**
 **********************************************************************************************
 * @file			EthConfig.h
 *
 * @brief			This file contain macro used to enable different code sets with different
 * 					functionality on ESP32 Ethernet Module.
 *
 * @details			These macros has been used in test_main.cpp file to call specific
 * 					functions
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef ETHCONFIG_H
#define ETHCONFIG_H

#define TOTAL_ETH_PORTS_IN_USE                  1		/* Total number of Ethernet ports*/

#define TRUSTED_IP_FILTER	/* Define this for inclusion of "Trusted IP filter" Feature for IP
							   protocols */

#ifdef ETHERNET_SETUP
#define __IP101GRI__
#endif

#ifdef __IP101GRI__
#undef TOTAL_ETH_PORTS_IN_USE
#define TOTAL_ETH_PORTS_IN_USE                  1		/* Total number of Ethernet ports*/
#define ETH_PHY_ADDR                            1		/* Ethernet PHY chip port address */
#define ETH_PHY_RST_GPIO                        5		/* Ethernet PHY chip RESET GPIO number */
#endif

#define ETH_MDC_GPIO                            23U		/* SMI MDC GPIO number */
#define ETH_MDIO_GPIO                           18U		/* SMI MDIO GPIO number */

#endif
