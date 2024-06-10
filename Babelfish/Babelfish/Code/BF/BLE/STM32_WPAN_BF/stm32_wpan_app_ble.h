/**
 ******************************************************************************
 * @file            stm32_wpan_app_ble.h
 * @brief           This file initilizes the stm32wb55 wpan ble stack and configures application specific data.
 *
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed here on. This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 * @copyright
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. All rights reserved.</center></h2>
 * This software component is licensed by ST under Ultimate Liberty license SLA0044, the "License"; You may not use this
 * file except in compliance with the License. You may obtain a copy of the License at: www.st.com/SLA0044
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_WPAN_APP_BLE_H
#define STM32_WPAN_APP_BLE_H

#include "BLE_DCI_Interface.h"
#include "BLE_Stack_Interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hci_tl.h"

/* Private includes ----------------------------------------------------------*/
#include "ble_types.h"

#ifdef __cplusplus
}
#endif

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief    Initialize the BLE stack.
 */
void PROD_SPEC_BLE_Init( void );

/**
 * @brief    Get Server connection status
 */
APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status( void );

/**
 * @brief    Initialize the BLE stack. This function is automatically called by the stack after the low-level
 * initialization has taken place.
 */
void APP_BLE_Init( void );

/**
 * @brief    Returns the max server data buffer size as mentioned in the ble_std.h file.
 */
uint16_t Get_Max_BLE_Server_Buffer_Size( void );

/**
 * @brief			Application updates the handles and other parameters for the dci and stack interface files.
 * @param[in]		ble_dci_interface_handle
 * @param[in]		ble_stack_interface_handler
 * @param[in]		ad_data
 * @param[in]		ad_data_size
 * @param[in]		device_name
 * @param[in]		device_name_length
 * @param[in]		ble_init_cmd_packet
 */
void Init_Interface_Parameters( BLE_DCI_Interface* ble_dci_interface_handle,
								BLE_Stack_Interface* ble_stack_interface_handler, uint8_t const* ad_data,
								uint8_t ad_data_size, const char* device_name, uint8_t device_name_length,
								SHCI_C2_Ble_Init_Cmd_Packet_t* ble_init_cmd_packet );


/**
 * @brief			Returns BLE Stack interface handle
 * @param[out]		BLE Stack interface handle
 */
BLE_Stack_Interface* Get_BLE_Stack_Interface_Handle( void );

/**
 * @brief		Start advertising
 */
void Adv_Request( void );

/**
 * @brief		Stop advertising
 */
void Adv_Stop( void );

/**
 * @brief		Start scanning for other BLE devices in vicinity
 */
void Discovery_Request( void );

/**
 * @brief		Stop scanning for BLE devices
 */
void Stop_Discovery( void );

/**
 * @brief		Set the TX power level
 */
void Set_TX_Power( uint8_t tx_power_level = CFG_TX_POWER );

static const uint8_t STM32_WPAN_TX_POWER_SET_MIN = 0U;
static const uint8_t STM32_WPAN_TX_POWER_SET_MAX = 0x1FU;
static const uint8_t STM32_WPAN_TX_PWR_TO_DB_COUNT = 0x20U;
static const int8_t STM32_WPAN_TX_PWR_TO_DB[STM32_WPAN_TX_PWR_TO_DB_COUNT] =
{
	-40,	// 0x00
	-21,	// 0x01
	-20,	// 0x02
	-19,	// 0x03
	-18,	// 0x04
	-17,	// 0x05
	-15,	// 0x06
	-14,	// 0x07
	-13,	// 0x08
	-12,	// 0x09
	-11,	// 0x0A
	-10,	// 0x0B
	-9,		// 0x0C
	-8,		// 0x0D
	-7,		// 0x0E
	-6,		// 0x0F
	-5,		// 0x10
	-4,		// 0x11
	-3,		// 0x12
	-2,		// 0x13
	-2,		// 0x14
	-1,		// 0x15
	-1,		// 0x16
	-1,		// 0x17
	0,		// 0x18
	0,		// 0x19
	1,		// 0x1A
	2,		// 0x1B
	3,		// 0x1C
	4,		// 0x1D
	5,		// 0x1E
	6		// 0x1F
};


#endif
