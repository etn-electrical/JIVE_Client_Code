/**
 *****************************************************************************************
 *	@file		Esp32_Ble_Api.h
 *
 *	@brief      Esp32 BLE interface implementation.
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
#ifndef ESP32_BLE_API_H
#define ESP32_BLE_API_H

#include "includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "Ble_Api.h"
#include "Ble_Defines.h"
#include "esp_err.h"
#ifdef CONFIG_BT_ENABLED	// Below code will get enabled only when BLE is enabled in sdkconfig
#include "esp_bt.h"
#endif	// #ifdef CONFIG_BT_ENABLED
#include "Eddystone_URL.h"
#include "Ble_Advertisement.h"
#include "Ble_DCI.h"
#include "DCI_Owner.h"
#include "DCI_Data.h"
#include "DCI_Defines.h"
#include "BLE_DCI_Data.h"
#include "Single_LED.h"

#define BLE_ADV_BUF_MAX_SIZE  ( 128 )	/// TODO: Fix this MAX_BUFFER with valid MTU size.

/**
 * @brief Start Advertising structure

 */
struct adv_start_struct_t
{
	esp_ble_adv_params_t* adv_params;
};

/**
 * @brief Advertising Data structure
 */
struct config_data_struct_t
{
	bool_t set_scan_rsp;								/*!< Set this advertising data as scan response or not*/
	bool_t include_name;								/*!< Advertising data include device name or not */
	bool_t include_txpower;								/*!< Advertising data include TX power */
	uint32_t min_interval;								/*!< Advertising data show slave preferred connection min
														   interval.
														   The connection interval in the following manner:
														   connIntervalmin = Conn_Interval_Min * 1.25 ms
														   Conn_Interval_Min range: 0x0006 to 0x0C80
														   Value of 0xFFFF indicates no specific minimum.
														   Values not defined above are reserved for future use.*/

	uint32_t max_interval;								/*!< Advertising data show slave preferred connection max
														   interval.
														   The connection interval in the following manner:
														   connIntervalmax = Conn_Interval_Max * 1.25 ms
														   Conn_Interval_Max range: 0x0006 to 0x0C80
														   Conn_Interval_Max shall be equal to or greater than the
														      Conn_Interval_Min.
														   Value of 0xFFFF indicates no specific maximum.
														   Values not defined above are reserved for future use.*/

	int32_t appearance;									/*!< External appearance of device */
	uint16_t manufacturer_len;							/*!< Manufacturer data length */
	uint8_t* p_manufacturer_data;						/*!< Manufacturer data point */
	uint16_t service_data_len;							/*!< Service data length */
	uint8_t* p_service_data;							/*!< Service data point */
	uint16_t service_uuid_len;							/*!< Service uuid length */
	uint8_t* p_service_uuid;							/*!< Service uuid array point */
	uint8_t flag;										/*!< Advertising flag of discovery mode, see BLE_ADV_DATA_FLAG
														   detail */
};

/**
 * @brief Encryption data structure
 */
struct encryption_param_struct_t
{
	esp_bd_addr_t bd_addr;
	esp_ble_sec_act_t sec_act;
};

/**
 * @brief Passkey structure
 */
struct passkey_param_struct_t
{
	esp_bd_addr_t bd_addr;
	bool_t accept;
	uint32_t passkey;
};

/**
 * @brief Bondable mode parameter structure
 */
struct bondable_mode_param_t
{
	uint16_t* dev_num;
	esp_ble_bond_dev_t* dev_list;
};

/**
 * @brief Connection parameter structure
 */
struct connection_param_struct_t
{
	esp_ble_conn_update_params_t* params;
	esp_bd_addr_t bd_addr;
	uint16_t min_interval;
	uint16_t max_interval;
	uint16_t latency;
	uint16_t timeout;
};

/**
 * @brief Remove Bonding structure
 */
struct remove_bond_struct_t
{
	esp_bd_addr_t bd_addr;
};

/**
 * @brief Read response structure
 */
struct read_response_struct_t
{
	esp_gatt_if_t gatts_if;
	uint16_t conn_id;
	uint32_t trans_id;
	esp_gatt_status_t status;
	esp_gatt_rsp_t* rsp;
};

/**
 * @brief Write response structure
 */
struct write_response_struct_t
{
	esp_gatt_if_t gatts_if;
	uint16_t conn_id;
	uint32_t trans_id;
	esp_gatt_status_t status;
	esp_gatt_rsp_t* rsp;
};

/**
 * @brief Gatts create attribute table structure
 */
struct attr_table_struct_t
{
	esp_gatts_attr_db_t* gatts_attr_db;
	esp_gatt_if_t gatts_if;
	uint8_t max_nb_attr;
	uint8_t srvc_inst_id;
};

/**
 * @brief Service structure
 */
struct start_service_struct_t
{
	uint16_t service_handle;
};

/**
 * @brief Pairing structure
 */
struct ble_oob_req_struct_t
{
	esp_bd_addr_t bd_addr;
	uint8_t* TK;
	uint8_t len;
};

/**
 * @brief BLE reply structure
 */
struct ble_confirm_reply_struct_t
{
	esp_bd_addr_t bd_addr;
	bool_t accept;
};

/**
 * @brief BLE connection structure
 */
struct connectable_mode_reply_struct_t
{
	esp_ble_conn_update_params_t* params;
};

/**
 * @brief ESP32 BLE Advertising Parameters
 */
static esp_ble_adv_params_t esp32_adv_config_params = {
	/*.adv_int_min        =*/ 0x100,
	/*.adv_int_max        = */ 0x100,
	/*.adv_type           = */ ADV_TYPE_IND,
	/*.own_addr_type      = */ BLE_ADDR_TYPE_PUBLIC,
	{0},
	( esp_ble_addr_type_t )( 0 ),
	/*.channel_map        = */ ADV_CHNL_ALL,
	/*.adv_filter_policy = */ ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY
};

/**
 * @brief Typedef for Attribute handle
 */
typedef uint16_t attribute_handle;

/**
 * @brief Save runtime parameters and attributes handle table structure
 */
typedef struct
{
	esp_gatt_if_t gatts_if;
	attribute_handle attrib_handle_tbl[MAX_GATTS_ATTIRIBUTE];	/// Store runtime handle for each attributes of service.
} gatts_profile_inst_t;

/*
 **********************************************************************************************
 * @brief Esp32_Ble_Api Class
 * @n			 This class is abstracted from the Ble_Api wrapper class
 * @n			 and this will provide interface for the ESP32 BLE Stack API's
 * @n		     for GATT server functionality
 * @details      It provides public methods
 **********************************************************************************************
 */
class Esp32_Ble_Api : public Ble_Api
{
	public:
		/**
		 * @brief                   Constructor to create instance of ESP32_BLE class.
		 * @param[in]               none
		 * @return					none
		 */

		Esp32_Ble_Api( Ble_Advertisement* ble_adv, Ble_DCI* ble_dci, Ble_User_Management* ble_user_mgmt,
					   Single_LED* ble_led = nullptr );
		/**
		 * @brief                   Destructor to delete the ESP32_Ble instance
		 * @n						when it goes out of scope
		 * @param[in]               none
		 * @return                  none
		 */
		~Esp32_Ble_Api( void );

		/**
		 * @brief                   Function to check if BLE channel is initialized and enabled
		 * @param[in]		        none
		 * @return                  none
		 */
		void Ble_Init( void );

		/**
		 * @brief                   Function for gap and gatt callbacks
		 * @param[in]		        none
		 * @return                  none
		 */
		void Register_Callbacks( void );

		/**
		 * @brief                   Function for initializing and enabling bluetooth
		 * @param[in]				none
		 * @return                  none
		 */
		void Init_Configuration( void );

		/**
		 * @brief                   Function to configure advertising data
		 * @param[in]				none
		 * @return                  none
		 */
		void Config_Adv_Data( void );

		/**
		 * @brief                   Function to configure raw advertising data for eddystone
		 * @param[in]				none
		 * @return					none
		 */
		void Config_Raw_Adv_Data( void );

		/**
		 * @brief                   Function to start advertising
		 * @param[in]		        none
		 * @return                  none
		 */
		void Start_Advertising( void* );

		/**
		 * @brief                   Function to configure security parameters
		 * @param[in]				void
		 * @return                  none
		 */
		void Security_Configure( void );

		/**
		 * @brief                   Function to set the Encryption
		 * @param[in]				void pointer
		 * @return                  none
		 */
		void Set_Encryption( void* );

		/**
		 * @brief                   Function to set bonding mode
		 * @param[in]				void pointer
		 * @return                  none
		 * @Note					For future reference
		 */
		void Set_Bondable_Mode( void* );

		/**
		 * @brief                   Configuring connection parameter
		 * @param[in]				void pointer
		 * @return                  none
		 * @Note					For future refernce
		 */
		void Connection_Parameter( void* );

		/**
		 * @brief                   Function to create attribute table
		 * @param[in]				void pointer
		 * @return                  none
		 */
		void Gatts_Create_Attr_Table( int, void* );

		/**
		 * @brief                   Function to create the GATT attribute table for the specified
		 *                          set of parameters provided via BLE Sheet on the DCI Workbook
		 * @param[in]               int32_t GATT interface handle to be created in the GATT Server
		 * @param[in]				void pointer
		 * @return                  none
		 */
		void Gatts_Start_Service( int32_t, void* );

		/**
		 * @brief                   Function to Read characteristic value of the BLE GATTS service
		 * @param[in]               int32_t GATT interface handle to be read from the target board via DCI interface
		 * @param[in]				void pointer
		 * @return                  none
		 */
		void Read_Event( int32_t, void* );

		/**
		 * @brief                   Function to Write characteristic value of the BLE GATTS service
		 * @param[in]               int32_t GATT interface handle to be write to the target board via DCI interface
		 * @param[in]				void pointer
		 * @return                  none
		 */
		void Write_Event( int32_t, void* );

		/**
		 * @brief                   Function to prepare the response for read request
		 * @param[in]				void pointer
		 * @return                  none
		 */
		void Read_Response( void* );

		/**
		 * @brief                   Function to prepare the response for write request
		 * @param[in]               void pointer
		 * @return                  none
		 */
		void Write_Response( void* );

		/**
		 * @brief                   Function for Authentication Complete
		 * @param[in]				void pointer
		 * @return                  none
		 */
		void Authentication_Complete( void* );

		/**
		 * @brief                   Function for Ble LED display
		 * @param[in]				void pointer
		 * @return                  none
		 */
		void Ble_Led_Display_Control( void );

	private:
		Ble_Advertisement* m_ble_adv;

		///< BLE DCI Object
		Ble_DCI* m_ble_dci;

		Single_LED* m_ble_led;

		///< BLE User Management Object
		Ble_User_Management* m_ble_user_mgmt;

		///< Buffer to store BLE DCI data
		uint8_t dest_data_read[BLE_ADV_BUF_MAX_SIZE];

		gatts_profile_inst_t gatts_profile_tab[TOTAL_SERVICE];

		/**
		 * @brief                   Function for converting Authentication request into string
		 * @param[in]				auth_req - Authentication request
		 * @return                  authentication request in string form
		 */
		char* Esp_Auth_Req_To_Str( esp_ble_auth_req_t auth_req );

		/**
		 * @brief                   Function for finding Service ID from Gatt If
		 * @param[in]				gatts_if -  saved during "GATTS_INTERFACE_CREATED" event for perticular services.
		 * @return                  none
		 */
		uint16_t Find_Service_Id_From_Gatt_If( esp_gatt_if_t gatts_if );

		/**
		 * @brief                   Function for mapping BLE stack error to esp-idf stack errors
		 * @param[in]				BLE error
		 * @return                  status of error
		 */
		esp_gatt_status_t Interpret_Ble_Error( uint8_t ble_error );


};

#endif	/*ESP32_BLE_AIPI_H*/
