/*
 * Delta_Com_Client.h
 *
 *  Created on: Sep 12, 2014
 *      Author: E9924418
 */

#ifndef DELTA_COM_CLIENT_H_
#define DELTA_COM_CLIENT_H_

#include "IDelta_Com_Client.h"
#include "Delta_Com_Identity.h"
#include "Delta_Data.h"
#include "Timers_Lib.h"

class IDelta_Com_Port;
class IDelta_Com_Manager;
class Delta_Com_Display;


class Delta_Com_Client : public IDelta_Com_Client
{
	public:
		Delta_Com_Client( uint8_t client_id, IDelta_Com_Manager* delta_com_manager,
						  IDelta_Com_Port* delta_com_port, uint8_t max_dcom_devices,
						  delta_com_identity_t* self_id_info );

		virtual delta_com_client_status Delta_Com_Port_Dev_State_Change( delta_com_device_address dcom_dev_address,
																		 delta_com_port_dev_state dcom_port_dev_state );

		virtual void Delta_Com_Response( uint8_t* data, uint16_t length,
										 delta_com_device_address dcom_dev_address );

		virtual void Disable_Client();

	private:

		static const uint8_t DCOM_CLIENT_MAX_RETRY_COUNT = 3U;
		typedef enum dcom_internal_state_t
		{
			DCOM_CLIENT_INTERNAL_NON_EXISTENT = 0,
			DCOM_CLIENT_INTERNAL_PORT_DEVICE_DISCOVERED,
			DCOM_CLIENT_INTERNAL_DEVICE_REMOVE,
			DCOM_CLIENT_INTERNAL_UNKNOWN_COMMAND,
			DCOM_CLIENT_INTERNAL_FRAMING_ERROR,
			DCOM_CLIENT_INTERNAL_START_IDENTITY_EXCHANGE,
			DCOM_CLIENT_INTERNAL_IDENTITY_REQUEST_SENT,
			DCOM_CLIENT_INTERNAL_IDENTITY_SERVER_VALIDATION_SUCCESS,
			DCOM_CLIENT_INTERNAL_IDENTITY_SERVER_VALIDATION_FAILURE,
			DCOM_CLIENT_INTERNAL_IDENTITY_CLIENT_VALIDATION_FAILURE,
			DCOM_CLIENT_INTERNAL_IDENTITY_MANAGER_VALIDATION_FAILURE,
			DCOM_CLIENT_INTERNAL_START_MANAGE_SUBSCRIPTIONS,
			DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_REQUEST_SENT,
			DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_SUCCESS_RESPONSE,
			DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_FAILURE_RESPONSE,
			DCOM_CLIENT_INTERNAL_START_SYNC_PARAMETERS,
			DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_REQUEST_SENT,
			DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_SUCCESS_RESPONSE,
			DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_FAILURE_RESPONSE,
			DCOM_CLIENT_INTERNAL_START_EXCHANGE_DELTA_DATA,
			DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_SENT,
			DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_RESENT,
			DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_SUCCESS_RESPONSE,
			DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_FAILURE_RESPONSE
		} dcom_internal_state;
		typedef struct Dcom_Client_Device_t
		{
			delta_com_client_dev_state m_dev_state;
			delta_com_device_address m_dcom_dev_address;
			delta_com_identity_t m_id_info;
			Delta_Data* m_delta_data;
			uint8_t m_local_sync_key;
			uint8_t m_remote_sync_key;
			uint8_t m_retry_count;
			dcom_internal_state m_dcom_internal_state;
			bool m_timedout_or_errored;
			bool m_request_ready;
			bool m_sync_completed;
		} Dcom_Client_Device;

		uint8_t m_client_id;
		uint8_t m_num_dcom_devices;
		IDelta_Com_Manager* m_delta_com_manager;
		IDelta_Com_Port* m_delta_com_port;
		delta_com_identity_t* m_self_id_info;
		BF_Lib::Timers* m_timer;
		uint16_t m_update_interval;

		uint16_t m_max_dcom_payload_len;
		uint8_t m_current_scan_index;
		Dcom_Client_Device* m_device_list;
		Dcom_Client_Device* Find_Dcom_Device( delta_com_device_address dcom_dev_address );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Dcom_Client_Scan_Task_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{ ( reinterpret_cast<Delta_Com_Client*>( handle ) )->Dcom_Client_Scan_Task(); }

		void Dcom_Client_Scan_Task();

		uint16_t Build_Manage_Subscriptions_Request( Dcom_Client_Device* dcom_device, uint8_t* tx_data ) const;

		bool Manage_Subscriptions_Response_Handler( Dcom_Client_Device* dcom_device, uint8_t* rx_data ) const;

		uint16_t Build_Identity_Exchange_Request( Dcom_Client_Device* dcom_device, uint8_t* tx_data ) const;

		void Identity_Exchange_Response_Handler( Dcom_Client_Device* dcom_device, uint8_t* rx_data ) const;

		uint16_t Build_Exchange_Delta_Parameters_Request( Dcom_Client_Device* dcom_device, uint8_t* tx_data ) const;

		Delta_Com::cmd_type_t Exchange_Delta_Parameters_Response_Handler( Dcom_Client_Device* dcom_device,
																		  uint8_t* rx_data ) const;

		bool Validate_Identity( Dcom_Client_Device* dcom_device ) const;

		void Remove_Device( Dcom_Client_Device* dcom_device );

		void Handle_Internal_State_Change( Dcom_Client_Device* dcom_device, dcom_internal_state past_state,
										   dcom_internal_state new_state );

};


#endif	/* DELTA_COM_CLIENT_H_ */
