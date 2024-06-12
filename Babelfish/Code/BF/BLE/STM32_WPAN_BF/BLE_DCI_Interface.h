/**
 ******************************************************************************
 * @file            BLE_DCI_Interface.h
 * @brief           BLE_DCI_Interface for STM32WB55 based BLE devices
 *
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed here on. This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 ******************************************************************************
 */

/*******************Define to prevent recursive inclusion ***********************/
#ifndef BLE_DCI_INTERFACE_H
#define BLE_DCI_INTERFACE_H
#include "BLE_DCI_Data.h"
#include "ble_types.h"

/**
 **************************************************************************************************
 *  @brief This class is responsible for transfering the logged data tO BLE stack interfaces.
 *  @details The callback for the Log DCIDs reads the data from Logging files.
 **************************************************************************************************
 */
class BLE_DCI_Interface
{
	public:
		typedef APP_BLE_ConnStatus_t BLE_STAT_CALLBACK_FUNC ( void );

		/**
		 * @brief      Constructs an instance of a BLE_DCI_Interface.
		 */
		BLE_DCI_Interface( Ble_DCI* ble_dci );

		/**
		 **********************************************************************************************
		 * @brief                           This function returns the Ble_DCI class object
		 * @return[out] Ble_DCI*			object of Ble_DCI class.
		 * @n
		 **********************************************************************************************
		 */
		tBleStatus Init( void );

		/**
		 **********************************************************************************************
		 * @brief                           This function returns the Ble_DCI class object
		 * @return[out] Ble_DCI*			object of Ble_DCI class.
		 * @n
		 **********************************************************************************************
		 */
		Ble_DCI* Get_Ble_Dci_Handler( void );

	private:
		BLE_DCI_Interface( const BLE_DCI_Interface& );				///< Private copy constructor
		BLE_DCI_Interface & operator =( const BLE_DCI_Interface& );				///< Private copy assignment operator

		void Load_Service_UUID( Service_UUID_t* uuidsrv, Ble_DCI::CHAR_ID_TD index, uint8_t* uuid_type );

		void Load_Char_UUID( Char_UUID_t* uuidchar, Ble_DCI::CHAR_ID_TD index, uint8_t* uuid_type );

		bool Find_Char_Attribute_Index( uint16_t char_attribute, Ble_DCI::CHAR_ID_TD* index_ret );

		bool Check_Zero( const uint8_t* str );

		bool Find_Index_From_Dcid( DCI_ID_TD dcid, Ble_DCI::CHAR_ID_TD* index_ret );

		uint16_t service_handles[DCI_BLE_DATA_TOTAL_CHAR_ID];
		uint16_t characteristic_handles[DCI_BLE_DATA_TOTAL_CHAR_ID];
		Ble_DCI* m_ble_dci;
		friend class BLE_Stack_Interface;
};

#endif
