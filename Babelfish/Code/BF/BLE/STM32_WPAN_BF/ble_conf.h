/**
 ******************************************************************************
 * File Name          : ble_conf.h
 * Description        : Configuration file for BLE Middleware.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BLE_CONF_H
#define BLE_CONF_H

#include "app_conf.h"

/******************************************************************************
*
* BLE SERVICES CONFIGURATION
* blesvc
*
******************************************************************************/

/**
 * This setting shall be set to '1' if the device needs to support the Peripheral Role
 * In the MS configuration, both BLE_CFG_PERIPHERAL and BLE_CFG_CENTRAL shall be set to '1'
 */
#ifndef BLE_CFG_PERIPHERAL
#define BLE_CFG_PERIPHERAL                                                     1
#endif

/**
 * This setting shall be set to '1' if the device needs to support the Central Role
 * In the MS configuration, both BLE_CFG_PERIPHERAL and BLE_CFG_CENTRAL shall be set to '1'
 */
#ifndef BLE_CFG_CENTRAL
#define BLE_CFG_CENTRAL                                                        0
#endif

/**
 * There is one handler per service enabled
 * Note: There is no handler for the Device Information Service
 *
 * This shall take into account all registered handlers
 * (from either the provided services or the custom services)
 */
#ifndef BLE_CFG_SVC_MAX_NBR_CB
#define BLE_CFG_SVC_MAX_NBR_CB                                                 7	// TODO: Change it to 1 after
																					// testing
#endif

#ifndef BLE_CFG_CLT_MAX_NBR_CB
#define BLE_CFG_CLT_MAX_NBR_CB                                                 0
#endif

/******************************************************************************
* GAP Service - Apprearance
******************************************************************************/

#define BLE_CFG_UNKNOWN_APPEARANCE                  ( 0 )
#define BLE_CFG_HR_SENSOR_APPEARANCE                ( 832 )

#ifndef BLE_CFG_GAP_APPEARANCE
#define BLE_CFG_GAP_APPEARANCE                      ( BLE_CFG_UNKNOWN_APPEARANCE )
#endif

#endif	/*BLE_CONF_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
