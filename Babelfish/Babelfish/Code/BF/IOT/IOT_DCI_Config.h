/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the DCI interface to IOT.
 *
 *	@details IOT_DCI contains all the necessary structures and methods to access the DCI interface.
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef IOT_DCI_CONFIG_H
#define IOT_DCI_CONFIG_H
#include "IoT_DCI.h"
#include "IoT_DCI_Data.h"
#include "DCI_Owner.h"
#include "iot_device.h"
#include "DCI_Enums.h"

/**
 ****************************************************************************************
 * @brief This is an IOT_DCI Configuration class
 * @details It defines owner and generate callbacks if any IOT_DCI configuration parameter is
 * changed.
 * @ingroup iot_dci_config
 *
 * @li It provides public methods to
 * @li 1. Initialize owners and attach callbacks.
 * @li 2. Update callback parameters.
 * @li 3. Set DCI_Change_Flag value.
 * @li 4. Get DCI_Change_Flag value.
 ****************************************************************************************
 */
class IOT_DCI_Config
{
	public:
		struct iot_config_param_t
		{
			DCI_Owner* iot_enable;
			DCI_Owner* iot_connection_status;
			DCI_Owner* proxy_enable;
			DCI_Owner* proxy_server_address;
			DCI_Owner* proxy_server_port;
			DCI_Owner* proxy_username;
			DCI_Owner* proxy_password;
			DCI_Owner* device_guid;
			DCI_Owner* device_profile;
			DCI_Owner* iot_connection_string;
			DCI_Owner* data_update_rate;
			DCI_Owner* cadence_update_rate;
			DCI_Owner* iot_conn_status_reason;
		};

		/**
		 * @brief Constructor.
		 * @details Initialize owners for each IOT_DCI config parameters and attach callbacks for
		 * the same.
		 * Also create an object for IOT_DCI class.
		 * @param config_struct The configuration structure describing the channel DCI to be constructed.
		 */
		IOT_DCI_Config( const iot_config_param_t* config_struct );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of IOT_DCI_Config goes out of scope
		 */
		~IOT_DCI_Config( void );

		/**
		 * @brief reset the m_DCI_change_flag.
		 * @details This function changes the m_DCI_change_flag back to 0.
		 */
		void Reset_Config_Change_Flag( void );

		/**
		 * @brief Get the m_DCI_change_flag.
		 * @details This function returns the value of m_DCI_change_flag.
		 * @return m_DCI_change_flag value.
		 */
		bool Is_Config_Change_Flag_Set( void );

		/**
		 * @brief Gets the IOT connection options structure.
		 * @details This function fills out and returns the IOT connection options needed for iot_open().
		 * @return Pointer to the Connection Options needed to open the IOT connection.
		 */
		IOT_CONNECTION_OPTIONS* Get_IOT_Connection_Options( void );

		/**
		 * @brief Get the IOT to cloud connection status.
		 * @details This function returns the status of cloud to device connection.
		 * @return true if connected, false if disconnected.
		 */
		bool Get_Connection_Status( void );

		/**
		 * @brief Set the IOT to cloud connection status.
		 * @details This function writes the status of cloud to device connection into DCID.
		 * @param set_val: Value to be set.
		 */
		void Set_Connection_Status( bool set_val );

		/**
		 * @brief Set the IOT to cloud connection status reason.
		 * @details This function writes the status reason of cloud to device connection into DCID.
		 * @param conn_status_reason: Value to be set.
		 */
		void Set_Conn_Status_Reason( dci_enum_uint8_t conn_status_reason );

		/**
		 * @brief Check if IOT is enable.
		 * @details This function returns weather IOT is enable or not.
		 * @return true if IOT is enable, false if IOT is disabled.
		 */
		bool Is_IOT_Enable( void );

		/** Getter for the Device UUID (aka GUID).
		 * Do not free the memory pointed to by the return value.
		 * @return Pointer to the Device UUID, in 8-4-4-4-12 text form.
		 */
		const char* Get_Device_Uuid( void );

		/** Getter for the Device's IoT Profile UUID.
		 * Do not free the memory pointed to by the return value.
		 * @return Pointer to the Profile UUID, in 8-4-4-4-12 text form.
		 */
		const char* Get_Device_Profile( void );

		/** Define a minimum valid string length to test against.
		 * Using 2 as a reasonable value - more than just 1 char.
		 */
		const uint8_t MIN_STRING_LEN_FOR_TEST = 2;

	private:
		/**
		 * @brief Updates the m_DCI_change_flag value.
		 * @details This function updates the m_DCI_change_flag value to 1.
		 * @param cback_param: Parameters for Callback function.
		 * @param access_struct: The access struct is the order slip.
		 * @return DCI callback return type.
		 */
		static DCI_CB_RET_TD Update_IOT_Config_CB_static( DCI_CBACK_PARAM_TD cback_param,
														  DCI_ACCESS_ST_TD* access_struct );

		DCI_CB_RET_TD Update_IOT_Config_CB( DCI_ACCESS_ST_TD* access_struct );

		/** @brief IOT dcid owner pointers*/
		DCI_Owner* m_owner_iot_enable;
		DCI_Owner* m_owner_iot_connection_status;
		DCI_Owner* m_owner_proxy_enable;
		DCI_Owner* m_owner_proxy_server_address;
		DCI_Owner* m_owner_proxy_server_port;
		DCI_Owner* m_owner_proxy_username;
		DCI_Owner* m_owner_proxy_password;
		DCI_Owner* m_owner_device_guid;
		DCI_Owner* m_owner_device_profile;
		DCI_Owner* m_owner_iot_connection_string;
		DCI_Owner* m_owner_data_update_rate;
		DCI_Owner* m_owner_cadence_update_rate;
		DCI_Owner* m_owner_iot_conn_status_reason;

		// Storage to hold the option strings read from the DCI
		uint8_t* m_device_uuid;
		uint8_t* m_device_profile;
		uint8_t* m_connection_string;
		uint8_t* m_proxy_address;
		uint8_t* m_proxy_username;
		uint8_t* m_proxy_password;

		/** Flag set if any of the IOT Configuration items have been changed. */
		bool m_DCI_change_flag;

		/** The structure of connection options to be given to the IoT SDK. */
		IOT_CONNECTION_OPTIONS m_connection_options;
};

#endif
