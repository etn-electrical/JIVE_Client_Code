/*
 * Modbus_Delta_Com_Client_Port.h
 *
 *  Created on: Aug 26, 2014
 *      Author: E9924418
 */

#ifndef MODBUS_DELTA_COM_CLIENT_PORT_H_
#define MODBUS_DELTA_COM_CLIENT_PORT_H_


#include "IDelta_Com_Port.h"
#include "IDelta_Com_Client.h"
#include "Modbus_Defines.h"
#include "Modbus_Master_No_OS.h"
#include "CR_Tasker.h"

namespace BF_Mbus
{
class Modbus_Master_No_OS;
}

class Modbus_Delta_Com_Client_Port : public IDelta_Com_Port
{
	public:
		Modbus_Delta_Com_Client_Port( uint8_t max_devices, uint8_t max_active_devices,
									  BF_Mbus::Modbus_Master_No_OS* modbus_master_no_os, uint32_t baud_rate,
									  uint8_t num_buffers = ONE_PER_DEVICE, bool_t auto_discovery_enabled = false );
		virtual void Delta_Com_Client_Set( IDelta_Com_Client* dcom_client );

		virtual delta_com_port_status Send_Data( delta_com_device_address address );

		virtual delta_com_port_status Get_TX_Buffer( delta_com_device_address address, uint8_t** buffer,
													 uint16_t** tx_length );

		virtual void Release_TX_Buffer( delta_com_device_address address );

		virtual void Remove_Device( delta_com_device_address address );

		virtual uint16_t Max_Payload_Size_Get();

		delta_com_port_status Add_Modbus_Device( uint8_t modbus_address );

		virtual uint16_t Max_Payload_Transmit_Time_Ms();

		virtual void Disable_Port();

	private:
		static const tx_mode_t DCOM_MODBUS_TX_MODE = MODBUS_RTU_TX_MODE;
		static const parity_t DCOM_MODBUS_PARITY_MODE = MODBUS_PARITY_EVEN;
		static const stop_bits_t DCOM_MODBUS_STOP_BITS = MODBUS_1_STOP_BIT;
		static const bool DCOM_MODBUS_LONG_HOLDOFF = false;

		static const uint16_t BITS_IN_BYTE = 11U;
		static const uint16_t MILLISECONDS_IN_SECOND = 1000U;
		static const uint16_t NEGOTIATION_FRAME_TIMEOUT_BYTES = 100U;
		static const uint16_t TIMEOUT_MULTIPLIER = 4U;

		typedef enum link_negotiation_state_t
		{
			LINK_NEGOTIATION_SET_MODBUS_ADDR_SENT = 0,
			LINK_NEGOTIATION_SET_MODBUS_ADDR_SUCCESS,
			LINK_NEGOTIATION_SET_MODBUS_ADDR_TIMEOUT_OR_ERROR,
			LINK_NEGOTIATION_BAUD_REQUEST_SENT,
			LINK_NEGOTIATION_BAUD_REQUEST_TIMEDOUT_OR_ERROR,
			LINK_NEGOTIATION_BAUD_REQUEST_SUCCESS
		} link_negotiation_state;

		typedef struct Modbus_Port_Device_t
		{
			uint8_t m_modbus_address;
			uint8_t m_tx_buffer_index;
			delta_com_port_dev_state m_dev_state;
			link_negotiation_state m_link_negotiation_state;
			bool_t m_auto_discovery_enabled;
			bool_t m_to_be_removed;
		} Modbus_Port_Device;

		static const uint8_t MODBUS_INVALID_DEVICE_ADDRESS = MODBUS_BROADCAST_ADDRESS;

		// Buffer defines
		static const uint8_t ONE_PER_DEVICE = 0U;
		static const uint8_t INVALID_BUFFER_INDEX = 0xFFU;
		typedef struct Modbus_Port_Buffer_t
		{
			uint8_t m_buffer[MODBUS_MAX_RTU_MSG_DATA_LEN];
			uint16_t m_length;
			bool_t m_in_use;
		} Modbus_Port_Buffer;

		void Modbus_Callback( uint8_t* data, uint16_t length, uint8_t mbus_return_status );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Modbus_Callback_Static( BF_Mbus::Modbus_Master_No_OS::response_callback_param_t handle,
											uint8_t* data, uint16_t length, uint8_t mbus_return_status )
		{
			( ( Modbus_Delta_Com_Client_Port* )handle )->Modbus_Callback( data, length,
																		  mbus_return_status );
		}

		void Process_Link_Message( uint8_t* data, uint16_t length, uint8_t error );

		void Re_Enable_Port( uint32_t baud );


		Modbus_Port_Buffer* m_buffer_list;
		Modbus_Port_Device* m_modbus_device_list;
		uint8_t m_num_devices;
		uint8_t m_num_buffers;
		BF_Mbus::Modbus_Master_No_OS* m_modbus_master_ctrl;
		static const uint16_t m_max_message_size = MODBUS_MAX_RTU_MSG_DATA_LEN;
		uint8_t m_rx_buffer[m_max_message_size];
		bool_t Is_Dcom_Address_Valid( delta_com_device_address address ) const;

		delta_com_device_address m_active_device;
		IDelta_Com_Client* m_dcom_client;
		uint8_t m_max_active_devices;
		uint8_t m_num_active_devices;
		uint32_t m_negotiation_baud_rate;
		uint32_t m_operational_baud_rate;
		uint32_t m_negotiation_message_timeout;
		uint32_t m_operational_message_timeout;

		// Scan parameters
		uint32_t m_current_scan_baud_rate;
		uint8_t m_current_scan_index;
		CR_Tasker* m_scan_task;
		void Device_Scan_Task();

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Device_Scan_Task_Static( CR_Tasker* tasker, CR_TASKER_PARAM param )
		{ ( reinterpret_cast<Modbus_Delta_Com_Client_Port*>( param ) )->Device_Scan_Task(); }

};



#endif	/* MODBUS_DELTA_COM_CLIENT_PORT_H_ */
