/**
 *****************************************************************************************
 *	@file		Ble_Api.h
 *
 *	@brief      Wrapper interface class for BLE GATT server
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BLE_API_H
#define BLE_API_H

#include "includes.h"
#include "DCI_Owner.h"
#include "Ble_Defines.h"
#include "Ble_Display.h"

/**
 **********************************************************************************************
 * @brief		Ble Class Declaration.
 * @n		    The class will provide interface to communicate BLE frames.
 * @details     It provides public methods to access BLE frames.
 **********************************************************************************************
 */
class Ble_Api
{
	public:
		static Ble_Api* m_ble_api_obj;
		/**
		 * @brief Gap Events Enumeration.
		 */
		enum gap_event_enum_t
		{
			SYSTEM_BOOT = 0,
			GAP_SET_ADV_DATA_COMPLETE,
			GAP_SECURITY_CONFIGURE_COMPLETE,
			GAP_SECURITY_REQUEST,
			GAP_PASSKEY_REQUEST,
			GAP_OOB_REQUEST,
			GAP_NUMERIC_COMPARISION_REQUEST,
			GAP_AUTH_COMPLETE,
			GAP_BONDING_SUCCESS,
			GAP_BONDING_FAILURE,
			GAP_REMOVE_BONDED_DEV_LIST,
			GAP_SET_RAW_ADV_DATA_COMPLETE,
			GAP_SET_RAW_SCAN_RSP_DATA_COMPLETE,
			GAP_MAX_EVENT
		};

		/**
		 * @brief Gatts Events Enumeration.
		 */
		enum gatts_event_enum_t
		{
			GATTS_ATTRIBUTE_TABLE_CREATED = 0,
			GATTS_CONNECT,
			GATTS_DISCONNECT,
			GATTS_READ,
			GATTS_WRITE,
			GATTS_READ_RESPONSE_COMPLETE,
			GATTS_WRITE_RESPONSE_COMPLETE,
			GATTS_INTERFACE_CREATED,
			GATTS_MAX_EVENT
		};

		static int32_t m_event_mapping_gap[GAP_MAX_EVENT];
		static int32_t m_event_mapping_gatts[GATTS_MAX_EVENT];

		/**
		 *	@brief      Constructor
		 *	@details	Does nothing
		 */
		Ble_Api( void ) {}

		/**
		 *  @brief      Destructor
		 *	@details	Does nothing
		 */
		~Ble_Api( void ) {}

		/**
		 * @brief       Function to check if BLE Initialization
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Init_Configuration( void ) = 0;

		/**
		 * @brief       Function to configure Raw Advertisement for Eddystone
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Config_Raw_Adv_Data( void ) = 0;

		/**
		 * @brief       Function to Start the Advertisement
		 * @param[in]	void none
		 * @return      none
		 */
		virtual void Start_Advertising( void* ) = 0;

		/**
		 * @brief       Function to call gap and gatt register callbacks
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Register_Callbacks( void ) = 0;

		/**
		 * @brief       Function to configure BLE Security
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Security_Configure( void ) = 0;

		/**
		 * @brief       Function to set BLE Encryption
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Set_Encryption( void* ) = 0;

		/**
		 * @brief       Function to Start the BLE GATTS service
		 * @param[in]   gatts_if: GATT interface for different application on GATT client
		 * @param[in]   param: pointer to param
		 * @return      none
		 */
		virtual void Gatts_Start_Service( int32_t gatts_if, void* param ) = 0;

		/**
		 * @brief       Function to Read characteristic value of the BLE GATTS service
		 * @param[in]   gatts_if: GATT interface for different application on GATT client
		 * @param[in]   param: pointer to param
		 * @return      none
		 */
		virtual void Read_Event( int32_t gatts_if, void* param ) = 0;

		/**
		 * @brief       Function to Write characteristic value of the BLE GATTS service
		 * @param[in]   gatts_if: GATT interface for different application on GATT client
		 * @param[in]   param: pointer to param
		 * @return      none
		 */
		virtual void Write_Event( int32_t gatts_if, void* param ) = 0;

		/**
		 * @brief       Function to prepare the response for read request
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Read_Response( void* ) = 0;

		/**
		 * @brief       Function to prepare the response for write request
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Write_Response( void* ) = 0;

		/// TODO
		/**
		 * @brief       Function to set the PassKey
		 * @param[in]   void none
		 * @return      none
		 * @Note		Function declared for future purpose
		 */
		void Set_Passkey( void* );

		/// TODO
		/**
		 * @brief       Function to Remove Bonding
		 * @param[in]   void none
		 * @return      none
		 * @Note		Function declared for future purpose
		 */
		void Remove_Bonding( void* );

		/**
		 * @brief       Function to configure the Advertise data
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Config_Adv_Data( void ) = 0;

		/**
		 * @brief       Function for authentication complete
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Authentication_Complete( void* ) = 0;

		/**
		 * @brief       Function to create Attribute table
		 * @param[in]   void none
		 * @return      none
		 */
		virtual void Gatts_Create_Attr_Table( int, void* ) = 0;

		/**
		 * @brief           Function to handle the gatts events.
		 * @param[in]	    EvtId: Event ID
		 * @param[in]       gatts_if GATT inferface for different application on GATT client
		 * @param[in]	    param: pointer to param
		 * @return	        none
		 */
		static void Ble_Event_Handler1( int32_t evt_id, int32_t gatts_if, void* param );

		/**
		 * @brief       Function to handle the gap events.
		 * @param[in]	EvtId: Event ID
		 * @param[in]	param: pointer to param
		 * @return      none
		 */
		static void Ble_Event_Handler2( int32_t evt_id, void* param );

		/**
		 * @brief                   function to handle LED Display control for BLE
		 * @param[in] void          none
		 * @return bool             none
		 */
		void Ble_Led_Display_Control( void );

		/**
		 * @brief pointer to store LED indication port for initialized hardware port
		 */
		Ble_Display* m_ble_display;
};

#endif	/* BLE_API_H */
