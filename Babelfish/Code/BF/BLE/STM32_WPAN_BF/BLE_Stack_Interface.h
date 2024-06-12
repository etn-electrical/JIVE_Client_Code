/**
 ******************************************************************************
 * @file            BLE_Stack_Interface.h
 * @brief           BLE Stack Interfaces for STM32WB55 based BLE devices This file has interfaces which transfers data
 * between DCI and ble stack
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed here on. This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 ******************************************************************************
 */
#ifndef BLE_STACK_INTERFACE_H
#define BLE_STACK_INTERFACE_H
#include "BLE_DCI_Data.h"
#include "app_conf.h"
#include "BLE_DCI_Interface.h"
#include "ble_types.h"

/**
 **************************************************************************************************
 *  @brief This class has ble stack interfaces
 *  @details This class has interfaces which transfers data between DCI and ble stack
 **************************************************************************************************
 */
class BLE_Stack_Interface
{
	public:
		/**
		 * @brief      Callback function typedef for connection status
		 */
		typedef APP_BLE_ConnStatus_t BLE_STAT_CALLBACK_FUNC ( void );

		/**
		 * @brief      Constructs an instance of a BLE_Stack_Interface.
		 */
		BLE_Stack_Interface( BLE_STAT_CALLBACK_FUNC* get_server_connection_status_cb,
							 BLE_DCI_Interface* ble_dci_interface_handle );

		/**
		 **********************************************************************************************
		 * @brief                           This function is process read request from BLE Client
		 * @param[in] char_attribute		characteristic attribute
		 * @param[in] offset  				offset
		 * @param[in] err_code				error code
		 * @return[out] bool				return status
		 * @n
		 **********************************************************************************************
		 */
		bool Read_Event( uint16_t char_attribute, uint16_t offset, tBleStatus* err_code );

		/**
		 **********************************************************************************************
		 * @brief                           This function is process write request from BLE Client
		 * @param[in] char_attribute        characteristic attribute
		 * @param[in] data					data pointer
		 * @param[in] err_code				error code
		 * @return[out] bool				return status
		 * @n
		 **********************************************************************************************
		 */
		bool Write_Event( uint16_t char_attribute, uint8_t* data, tBleStatus* err_code );

	private:
		BLE_Stack_Interface( const BLE_Stack_Interface& );				///< Private copy constructor
		BLE_Stack_Interface & operator =( const BLE_Stack_Interface& );				///< Private copy assignment

		///< operator

		void Change_Track_Handler( DCI_ID_TD dcid );

		static void Change_Track_Handler_Static( Change_Tracker::cback_param_t handle, DCI_ID_TD dcid,
												 Change_Tracker::attrib_mask_t attribute_mask )
		{
			( ( BLE_Stack_Interface* )handle )->Change_Track_Handler( dcid );
		}

		tBleStatus Ble_Char_Value_Update( uint16_t serv_handle, uint16_t char_handle,
										  uint16_t offset, uint16_t length, uint8_t* data );

		BLE_STAT_CALLBACK_FUNC* m_get_server_connection_status_cb;
		bool notification_enable[DCI_BLE_DATA_TOTAL_CHAR_ID];
		Change_Tracker* m_change_tracker;
		uint8_t dest_data_read[MAX_BUFFER];
		uint8_t dest_data_change_track[MAX_BUFFER];
		BLE_DCI_Interface* m_ble_dci_interface_handle;
};

#endif
