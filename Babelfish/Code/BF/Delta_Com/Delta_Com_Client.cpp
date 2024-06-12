/*
 * Delta_Com_Client.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: E9924418
 */

#include "Includes.h"
#include "Delta_Com_Client.h"
#include "Delta_Com_Display.h"
#include "IDelta_Com_Manager.h"
#include "Ram.h"
#include "Data_Align.hpp"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Client::Delta_Com_Client( uint8_t client_id, IDelta_Com_Manager* delta_com_manager,
									IDelta_Com_Port* delta_com_port, uint8_t max_dcom_devices,
									delta_com_identity_t* self_id_info ) :
	m_delta_com_manager( delta_com_manager ),
	m_delta_com_port( delta_com_port ),
	m_client_id( client_id ),
	m_num_dcom_devices( max_dcom_devices ),
	m_current_scan_index( 0U ),
	m_self_id_info( self_id_info ),
	m_timer( nullptr ),
	m_update_interval( 0U ),
	m_max_dcom_payload_len( 0U ),
	m_device_list( nullptr )
{
	m_device_list = reinterpret_cast<Dcom_Client_Device*>(
		Ram::Allocate( sizeof( Dcom_Client_Device ) * max_dcom_devices ) );

	m_delta_com_port->Delta_Com_Client_Set( this );

	for ( uint8_t dev_index = 0U; dev_index < m_num_dcom_devices; dev_index++ )
	{
		Remove_Device( &m_device_list[dev_index] );
	}

	m_max_dcom_payload_len = m_delta_com_port->Max_Payload_Size_Get();
	m_self_id_info->m_buffer_size = m_max_dcom_payload_len;

	m_update_interval = m_delta_com_port->Max_Payload_Transmit_Time_Ms();

	if ( m_update_interval < m_self_id_info->m_update_interval_time )
	{
		m_update_interval = m_self_id_info->m_update_interval_time;
	}
	else
	{
		m_self_id_info->m_update_interval_time = m_update_interval;
	}

	m_timer = new BF_Lib::Timers( &Dcom_Client_Scan_Task_Static, this,
								  BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "Dcom Client Scan Task" );
	m_timer->Start( m_update_interval, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Client::Remove_Device( Dcom_Client_Device* dcom_device )
{
	m_delta_com_port->Remove_Device( dcom_device->m_dcom_dev_address );
	if ( dcom_device->m_delta_data != nullptr )
	{
		dcom_device->m_delta_data->Reset_Full();
	}
	dcom_device->m_dcom_dev_address = INVALID_DELTA_COM_DEVICE_ADDRESS;
	dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_NON_EXISTENT;
	dcom_device->m_delta_data = nullptr;
	dcom_device->m_dev_state = DCOM_CLIENT_DEV_NON_EXISTENT;
	dcom_device->m_id_info.m_buffer_size = 0U;
	dcom_device->m_id_info.m_device_serial_number = 0U;
	dcom_device->m_id_info.m_firmware_revision = 0U;
	dcom_device->m_id_info.m_hardware_revision = 0U;
	dcom_device->m_id_info.m_identity_revision = 0U;
	dcom_device->m_id_info.m_product_code = 0U;
	dcom_device->m_id_info.m_product_compatibility_number = 0U;
	dcom_device->m_id_info.m_product_subcode = 0U;
	dcom_device->m_id_info.m_update_interval_time = 0U;
	dcom_device->m_local_sync_key = Delta_Com::INITIAL_SYNC_KEY;
	dcom_device->m_remote_sync_key = Delta_Com::INITIAL_SYNC_KEY;
	dcom_device->m_request_ready = false;
	dcom_device->m_retry_count = 0U;
	dcom_device->m_timedout_or_errored = false;
	dcom_device->m_sync_completed = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_client_status Delta_Com_Client::Delta_Com_Port_Dev_State_Change(
	delta_com_device_address dcom_dev_address,
	delta_com_port_dev_state dcom_port_dev_state )
{
	Delta_Com_Client::Dcom_Client_Device* dcom_device;

	dcom_device = Find_Dcom_Device( dcom_dev_address );
	uint8_t dev_index;
	delta_com_client_status status = DCOM_CLIENT_SUCCESS;

	switch ( dcom_port_dev_state )
	{
		case DCOM_PORT_DEV_NON_EXISTENT:
			// Device is lost or removed
			if ( nullptr != dcom_device )
			{
				m_device_list[m_current_scan_index].m_dcom_internal_state =
					DCOM_CLIENT_INTERNAL_DEVICE_REMOVE;
			}
			break;

		case DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK:
		case DCOM_PORT_DEV_ESTABLISHING_LINK:
		case DCOM_PORT_DEV_TRANSACTING:
			break;

		case DCOM_PORT_DEV_IDLE:
			if ( nullptr == dcom_device )	// It's a newly discovered device
			{
				for ( dev_index = 0U; dev_index < m_num_dcom_devices; dev_index++ )
				{
					if ( DCOM_CLIENT_INTERNAL_NON_EXISTENT ==
						 m_device_list[m_current_scan_index].m_dcom_internal_state )
					{
						break;
					}
				}
				if ( dev_index >= m_num_dcom_devices )
				{
					status = DCOM_CLIENT_OUT_OF_RESOURCE;
				}
				else
				{
					m_device_list[dev_index].m_dcom_dev_address = dcom_dev_address;
					m_device_list[m_current_scan_index].m_dcom_internal_state =
						DCOM_CLIENT_INTERNAL_PORT_DEVICE_DISCOVERED;
				}
			}
			break;

		case DCOM_PORT_DEV_TIMEOUT:
		case DCOM_PORT_DEV_ERROR:
		default:
			if ( nullptr != dcom_device )
			{
				dcom_device->m_timedout_or_errored = true;
			}
			break;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Client::Dcom_Client_Device* Delta_Com_Client::Find_Dcom_Device( delta_com_device_address dcom_dev_address )
{
	Delta_Com_Client::Dcom_Client_Device* dcom_device = nullptr;

	for ( uint8_t dev_index = 0U; dev_index < m_num_dcom_devices; dev_index++ )
	{
		if ( dcom_dev_address == m_device_list[dev_index].m_dcom_dev_address )
		{
			dcom_device = &m_device_list[dev_index];
		}
	}

	return ( dcom_device );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Delta_Com_Client::Build_Manage_Subscriptions_Request( Dcom_Client_Device* dcom_device, uint8_t* tx_data ) const
{
	uint16_t tx_frame_itr;
	uint16_t remaining_tx_len;
	uint16_t tx_length_used;

	tx_data[COMMAND_BYTE] = Delta_Com::MANAGE_SUBSCRIPTIONS;

	tx_frame_itr = MANAGE_SUBSCRIPTIONS_FRAME_BEGIN;

	remaining_tx_len = m_max_dcom_payload_len - tx_frame_itr;
	dcom_device->m_delta_data->Subscription_Req_List( &tx_data[tx_frame_itr],
													  &tx_length_used, remaining_tx_len,
													  CLIENT_PUB_REQ_CLIENT_SOURCE_OF_TRUTH );
	tx_frame_itr += tx_length_used;

	remaining_tx_len = m_max_dcom_payload_len - tx_frame_itr;
	dcom_device->m_delta_data->Subscription_Req_List( &tx_data[tx_frame_itr],
													  &tx_length_used, remaining_tx_len,
													  CLIENT_PUB_REQ_SERVER_SOURCE_OF_TRUTH );
	tx_frame_itr += tx_length_used;

	Data_Align<uint16_t>::To_Array( tx_frame_itr, &tx_data[LENGTH_LOW] );

	return ( tx_frame_itr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Com_Client::Manage_Subscriptions_Response_Handler( Dcom_Client_Device* dcom_device, uint8_t* rx_data ) const
{
	uint16_t rx_frame_itr;
	uint8_t publish_req_type;
	uint16_t number_of_parameters;
	bool delta_com_frag_st;
	bool subscription_complete = false;

	rx_frame_itr = MANAGE_SUBSCRIPTIONS_FRAME_BEGIN;
	publish_req_type = rx_data[rx_frame_itr];
	rx_frame_itr += ATTRIBUTE_LENGTH;

	number_of_parameters = Data_Align<uint16_t>::From_Array( &rx_data[rx_frame_itr] );
	rx_frame_itr += COUNT_LENGTH;

	if ( number_of_parameters > 0U )
	{
		dcom_device->m_delta_data->Publish_Request( &rx_data[rx_frame_itr],
													number_of_parameters, publish_req_type );
	}
	else
	{
		number_of_parameters = 0U;
	}

	// Was client fragmented?  Meaning it still had stuff to send.
	// Are we done subscribing?  If client is done and server are done then consider
	// it initialized.
	delta_com_frag_st = ( !Delta_Com::Is_Fragmented( rx_data[COMMAND_BYTE] ) );
	if ( dcom_device->m_delta_data->Subscription_Complete() &&
		 delta_com_frag_st )
	{
		subscription_complete = true;
	}

	return ( subscription_complete );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Delta_Com_Client::Build_Exchange_Delta_Parameters_Request( Dcom_Client_Device* dcom_device,
																	uint8_t* tx_data ) const
{
	uint16_t data_length_retrieved;
	uint16_t tx_length;

	tx_data[COMMAND_BYTE] = static_cast<uint8_t>( Delta_Com::EXCHANGE_DELTA_PARAMETERS );

	dcom_device->m_delta_data->Get_Delta_Data( &tx_data[SET_DELTA_FRAME_BEGIN], &data_length_retrieved,
											   ( m_max_dcom_payload_len - SET_DELTA_FRAME_BEGIN ) );
	tx_length = SET_DELTA_FRAME_BEGIN + data_length_retrieved;

	tx_data[SET_DELTA_PREVIOUS_DE_QUE_KEY] = dcom_device->m_remote_sync_key;

	/// Calculate the new key to send and store it for when we get a response.
	dcom_device->m_local_sync_key++;
	if ( dcom_device->m_local_sync_key == Delta_Com::INITIAL_SYNC_KEY )
	{
		dcom_device->m_local_sync_key = 0U;
	}
	tx_data[SET_DELTA_NEW_DE_QUE_KEY] = dcom_device->m_local_sync_key;

	Data_Align<uint16_t>::To_Array( tx_length, &tx_data[LENGTH_LOW] );

	return ( tx_length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Client::Dcom_Client_Scan_Task()
{
	uint8_t* tx_buffer;
	uint16_t* tx_length;
	delta_com_port_status port_status;
	dcom_internal_state past_state = m_device_list[m_current_scan_index].m_dcom_internal_state;
	bool resend_data = false;
	bool forced_state_update = false;

	if ( ( m_device_list[m_current_scan_index].m_retry_count > DCOM_CLIENT_MAX_RETRY_COUNT ) &&
		 ( m_device_list[m_current_scan_index].m_timedout_or_errored ) )
	{
		m_device_list[m_current_scan_index].m_retry_count = 0U;
		m_device_list[m_current_scan_index].m_dcom_internal_state = DCOM_CLIENT_INTERNAL_DEVICE_REMOVE;
	}
	else
	{
		switch ( m_device_list[m_current_scan_index].m_dcom_internal_state )
		{
			case DCOM_CLIENT_INTERNAL_PORT_DEVICE_DISCOVERED:
				m_device_list[m_current_scan_index].m_dcom_internal_state =
					DCOM_CLIENT_INTERNAL_START_IDENTITY_EXCHANGE;
				break;

			case DCOM_CLIENT_INTERNAL_START_IDENTITY_EXCHANGE:
				port_status = m_delta_com_port->Get_TX_Buffer(
					m_device_list[m_current_scan_index].m_dcom_dev_address,
					&tx_buffer, &tx_length );
				if ( DCOM_PORT_SUCCESS == port_status )
				{
					if ( !m_device_list[m_current_scan_index].m_request_ready )
					{
						// populate the tx_buffer and tx_length with the identity request.
						*tx_length = Build_Identity_Exchange_Request(
							&m_device_list[m_current_scan_index], tx_buffer );
						m_device_list[m_current_scan_index].m_request_ready = true;
					}
					port_status = m_delta_com_port->Send_Data(
						m_device_list[m_current_scan_index].m_dcom_dev_address );
					if ( DCOM_PORT_SUCCESS == port_status )
					{
						m_device_list[m_current_scan_index].m_dcom_internal_state =
							DCOM_CLIENT_INTERNAL_IDENTITY_REQUEST_SENT;
						m_device_list[m_current_scan_index].m_retry_count = 1U;
						m_device_list[m_current_scan_index].m_timedout_or_errored = false;
					}
				}
				break;

			case DCOM_CLIENT_INTERNAL_IDENTITY_REQUEST_SENT:
				if ( m_device_list[m_current_scan_index].m_timedout_or_errored )
				{
					resend_data = true;
				}
				break;

			case DCOM_CLIENT_INTERNAL_IDENTITY_SERVER_VALIDATION_SUCCESS:
				if ( !Validate_Identity( &m_device_list[m_current_scan_index] ) )
				{
					m_device_list[m_current_scan_index].m_dcom_internal_state =
						DCOM_CLIENT_INTERNAL_IDENTITY_CLIENT_VALIDATION_FAILURE;
				}
				else
				{
					m_device_list[m_current_scan_index].m_delta_data = m_delta_com_manager
						->New_Delta_Com_Device_Add( m_client_id,
													m_device_list[m_current_scan_index].m_dcom_dev_address,
													&m_device_list[m_current_scan_index].m_id_info );
					if ( nullptr == m_device_list[m_current_scan_index].m_delta_data )
					{
						m_device_list[m_current_scan_index].m_dcom_internal_state =
							DCOM_CLIENT_INTERNAL_IDENTITY_MANAGER_VALIDATION_FAILURE;
					}
					else
					{
						m_device_list[m_current_scan_index].m_dcom_internal_state =
							DCOM_CLIENT_INTERNAL_START_MANAGE_SUBSCRIPTIONS;
					}
				}
				break;

			case DCOM_CLIENT_INTERNAL_START_MANAGE_SUBSCRIPTIONS:
			case DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_SUCCESS_RESPONSE:
				port_status = m_delta_com_port->Get_TX_Buffer(
					m_device_list[m_current_scan_index].m_dcom_dev_address,
					&tx_buffer, &tx_length );
				if ( DCOM_PORT_SUCCESS == port_status )
				{
					if ( !m_device_list[m_current_scan_index].m_request_ready )
					{
						// populate the tx_buffer and tx_length with the Manage subscriptions request.
						*tx_length = Build_Manage_Subscriptions_Request(
							&m_device_list[m_current_scan_index], tx_buffer );
						m_device_list[m_current_scan_index].m_request_ready = true;
					}
					port_status = m_delta_com_port->Send_Data(
						m_device_list[m_current_scan_index].m_dcom_dev_address );
					if ( DCOM_PORT_SUCCESS == port_status )
					{
						m_device_list[m_current_scan_index].m_dcom_internal_state =
							DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_REQUEST_SENT;
						m_device_list[m_current_scan_index].m_retry_count = 1U;
						m_device_list[m_current_scan_index].m_timedout_or_errored = false;
					}
				}
				break;

			case DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_REQUEST_SENT:
				if ( m_device_list[m_current_scan_index].m_timedout_or_errored )
				{
					resend_data = true;
				}
				break;

			case DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_FAILURE_RESPONSE:
				m_device_list[m_current_scan_index].m_dcom_internal_state =
					DCOM_CLIENT_INTERNAL_DEVICE_REMOVE;
				break;

			case DCOM_CLIENT_INTERNAL_START_SYNC_PARAMETERS:
			case DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_SUCCESS_RESPONSE:
			case DCOM_CLIENT_INTERNAL_START_EXCHANGE_DELTA_DATA:
			case DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_SUCCESS_RESPONSE:
				if ( ( DCOM_CLIENT_INTERNAL_START_SYNC_PARAMETERS ==
					   m_device_list[m_current_scan_index].m_dcom_internal_state )
					 ||
					 ( DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_SUCCESS_RESPONSE ==
					   m_device_list[m_current_scan_index].m_dcom_internal_state ) )
				{
					if ( ( m_device_list[m_current_scan_index].m_delta_data->Sync_Complete() ) &&
						 ( m_device_list[m_current_scan_index].m_sync_completed ) )
					{
						m_device_list[m_current_scan_index].m_dcom_internal_state =
							DCOM_CLIENT_INTERNAL_START_EXCHANGE_DELTA_DATA;
					}
				}

				port_status = m_delta_com_port->Get_TX_Buffer(
					m_device_list[m_current_scan_index].m_dcom_dev_address,
					&tx_buffer, &tx_length );
				if ( DCOM_PORT_SUCCESS == port_status )
				{
					if ( !m_device_list[m_current_scan_index].m_request_ready )
					{
						// populate the tx_buffer and tx_length with the delta data exchange request.
						*tx_length = Build_Exchange_Delta_Parameters_Request(
							&m_device_list[m_current_scan_index], tx_buffer );
						m_device_list[m_current_scan_index].m_request_ready = true;
					}
					port_status = m_delta_com_port->Send_Data(
						m_device_list[m_current_scan_index].m_dcom_dev_address );
					if ( DCOM_PORT_SUCCESS == port_status )
					{
						if ( ( DCOM_CLIENT_INTERNAL_START_SYNC_PARAMETERS ==
							   m_device_list[m_current_scan_index].m_dcom_internal_state )
							 ||
							 ( DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_SUCCESS_RESPONSE ==
							   m_device_list[m_current_scan_index].m_dcom_internal_state ) )
						{
							m_device_list[m_current_scan_index].m_dcom_internal_state =
								DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_REQUEST_SENT;
						}
						else if ( DCOM_CLIENT_INTERNAL_START_EXCHANGE_DELTA_DATA ==
								  m_device_list[m_current_scan_index].m_dcom_internal_state )
						{
							m_device_list[m_current_scan_index].m_dcom_internal_state =
								DCOM_CLIENT_INTERNAL_START_EXCHANGE_DELTA_DATA;
						}
						else
						{
							m_device_list[m_current_scan_index].m_dcom_internal_state =
								DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_SENT;
						}

						m_device_list[m_current_scan_index].m_retry_count = 1U;
						m_device_list[m_current_scan_index].m_timedout_or_errored = false;
					}
				}
				break;

			case DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_REQUEST_SENT:
			case DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_SENT:
			case DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_RESENT:
				if ( m_device_list[m_current_scan_index].m_timedout_or_errored )
				{
					if ( ( m_device_list[m_current_scan_index].m_dcom_internal_state ==
						   DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_SENT )
						 ||
						 ( m_device_list[m_current_scan_index].m_dcom_internal_state ==
						   DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_RESENT ) )
					{
						m_device_list[m_current_scan_index].m_dcom_internal_state =
							DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_RESENT;
						forced_state_update = true;
					}
					resend_data = true;
				}
				break;

			case DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_FAILURE_RESPONSE:
			case DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_FAILURE_RESPONSE:
				m_device_list[m_current_scan_index].m_dcom_internal_state =
					DCOM_CLIENT_INTERNAL_DEVICE_REMOVE;
				break;

			case DCOM_CLIENT_INTERNAL_NON_EXISTENT:
				// Nothing to do
				break;

			case DCOM_CLIENT_INTERNAL_DEVICE_REMOVE:
				Remove_Device( &m_device_list[m_current_scan_index] );
				m_device_list[m_current_scan_index].m_dcom_internal_state =
					DCOM_CLIENT_INTERNAL_NON_EXISTENT;
				break;

			case DCOM_CLIENT_INTERNAL_UNKNOWN_COMMAND:
			case DCOM_CLIENT_INTERNAL_FRAMING_ERROR:
				m_device_list[m_current_scan_index].m_dcom_internal_state =
					DCOM_CLIENT_INTERNAL_DEVICE_REMOVE;
				break;

			case DCOM_CLIENT_INTERNAL_IDENTITY_CLIENT_VALIDATION_FAILURE:
			case DCOM_CLIENT_INTERNAL_IDENTITY_MANAGER_VALIDATION_FAILURE:
			case DCOM_CLIENT_INTERNAL_IDENTITY_SERVER_VALIDATION_FAILURE:
			default:
				m_device_list[m_current_scan_index].m_dcom_internal_state =
					DCOM_CLIENT_INTERNAL_DEVICE_REMOVE;
				break;
		}

		if ( resend_data )
		{
			port_status = m_delta_com_port->Send_Data( m_device_list[m_current_scan_index].m_dcom_dev_address );
			if ( DCOM_PORT_SUCCESS == port_status )
			{
				m_device_list[m_current_scan_index].m_retry_count++;
				m_device_list[m_current_scan_index].m_timedout_or_errored = false;
			}
		}
	}

	if ( ( past_state != m_device_list[m_current_scan_index].m_dcom_internal_state ) || forced_state_update )
	{
		Handle_Internal_State_Change( &m_device_list[m_current_scan_index], past_state,
									  m_device_list[m_current_scan_index].m_dcom_internal_state );
	}

	// Move on to the next device
	m_current_scan_index++;

	if ( m_current_scan_index >= m_num_dcom_devices )
	{
		m_current_scan_index = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com::cmd_type_t Delta_Com_Client::Exchange_Delta_Parameters_Response_Handler( Dcom_Client_Device* dcom_device,
																					uint8_t* rx_data ) const
{
	// If return is fragmented re-send the command to get more data.
	Delta_Com::cmd_type_t command_type;
	uint16_t rx_frame_len;
	delta_com_error_t set_stat;

	command_type = Delta_Com::Get_Command_Type( rx_data[COMMAND_BYTE] );

	BF_ASSERT( command_type != Delta_Com::PARAMETER_MISMATCH );

	// The Client sent a parameter that the Server did not understand
	// This should have never happened because the client shall send only the parameters that were
	// subscribed to by the server
	if ( command_type == Delta_Com::SUCCESSFUL_RESPONSE )
	{
		if ( dcom_device->m_local_sync_key == rx_data[GET_DELTA_PREVIOUS_DE_QUE_KEY] )
		{
			rx_frame_len = Data_Align<uint16_t>::From_Array( &rx_data[LENGTH_LOW] );

			set_stat = dcom_device->m_delta_data->Set_Delta_Data( &rx_data[GET_DELTA_FRAME_BEGIN],
																  rx_frame_len - GET_DELTA_FRAME_BEGIN );
			BF_ASSERT( NO_DCOM_ERROR == set_stat );

			// The parameters have been agreed upon during the manage subscriptions.
			// Server cannot send a parameter that the client does not understand
			dcom_device->m_remote_sync_key = rx_data[GET_DELTA_NEW_DE_QUE_KEY];
		}
		else		// Indicates that the wrong sync key was returned.
		{			// Client is out of sync
			command_type = Delta_Com::SERVER_OUT_OF_SYNC;
		}
	}
	else
	{
		// do nothing
	}

	return ( command_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Client::Disable_Client()
{
	m_delta_com_port->Disable_Port();
	m_timer->Stop();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Delta_Com_Client::Build_Identity_Exchange_Request( Dcom_Client_Device* dcom_device, uint8_t* tx_data ) const
{
	uint16_t tx_length = IDENTITY_REQUEST_FRAME_LENGTH;

	tx_data[COMMAND_BYTE] = static_cast<uint8_t>( Delta_Com::EXCHANGE_IDENTITY );
	tx_data[IDENTITY_REQUEST_FRAME_ID_BLOCK_COMPATIBILITY] = m_self_id_info->m_identity_revision;
	tx_data[IDENTITY_REQUEST_FRAME_PROD_COMPATIBILITY] = m_self_id_info->m_product_compatibility_number;
	Data_Align<uint16_t>::To_Array( m_self_id_info->m_product_code, &tx_data[IDENTITY_REQUEST_FRAME_PROD_CODE_0] );
	Data_Align<uint16_t>::To_Array( m_self_id_info->m_product_subcode,
									&tx_data[IDENTITY_REQUEST_FRAME_SUB_PROD_CODE_0] );
	Data_Align<uint32_t>::To_Array( m_self_id_info->m_firmware_revision, &tx_data[IDENTITY_REQUEST_FRAME_FW_REV_0] );
	Data_Align<uint16_t>::To_Array( m_self_id_info->m_hardware_revision, &tx_data[IDENTITY_REQUEST_FRAME_HW_REV_0] );
	Data_Align<uint32_t>::To_Array( m_self_id_info->m_device_serial_number,
									&tx_data[IDENTITY_REQUEST_FRAME_SERIAL_NUM_0] );
	Data_Align<uint16_t>::To_Array( m_self_id_info->m_buffer_size, &tx_data[IDENTITY_REQUEST_FRAME_BUFFER_SIZE_0] );
	Data_Align<uint16_t>::To_Array( m_update_interval, &tx_data[IDENTITY_REQUEST_FRAME_UPDATE_INTERVAL_TIME_0] );
	Data_Align<uint16_t>::To_Array( tx_length, &tx_data[LENGTH_LOW] );

	return ( tx_length );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'dcom_device' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Client::Identity_Exchange_Response_Handler( Dcom_Client_Device* dcom_device, uint8_t* rx_data ) const
{
	dcom_device->m_id_info.m_identity_revision = rx_data[IDENTITY_REQUEST_FRAME_ID_BLOCK_COMPATIBILITY];
	dcom_device->m_id_info.m_product_compatibility_number = rx_data[IDENTITY_REQUEST_FRAME_PROD_COMPATIBILITY];
	dcom_device->m_id_info.m_product_code =
		Data_Align<uint16_t>::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_PROD_CODE_0] );
	dcom_device->m_id_info.m_product_subcode =
		Data_Align<uint16_t>::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_SUB_PROD_CODE_0] );
	dcom_device->m_id_info.m_firmware_revision =
		Data_Align<uint32_t>::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_FW_REV_0] );
	dcom_device->m_id_info.m_hardware_revision =
		Data_Align<uint16_t>::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_HW_REV_0] );
	dcom_device->m_id_info.m_device_serial_number =
		Data_Align<uint32_t>::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_SERIAL_NUM_0] );
	dcom_device->m_id_info.m_buffer_size =
		Data_Align<uint16_t>::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_BUFFER_SIZE_0] );
	dcom_device->m_id_info.m_update_interval_time =
		Data_Align<uint16_t>::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_UPDATE_INTERVAL_TIME_0] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Com_Client::Validate_Identity( Dcom_Client_Device* dcom_device ) const
{
	bool validation_success;

	if ( ( m_self_id_info->m_identity_revision != dcom_device->m_id_info.m_identity_revision ) ||
		 ( m_self_id_info->m_update_interval_time != dcom_device->m_id_info.m_update_interval_time ) ||
		 ( dcom_device->m_id_info.m_buffer_size < m_max_dcom_payload_len ) )
	{
		validation_success = false;
	}
	else
	{
		validation_success = true;
	}
	return ( validation_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Client::Handle_Internal_State_Change( Dcom_Client_Device* dcom_device, dcom_internal_state past_state,
													 dcom_internal_state new_state )
{
	bool send_update = true;

	switch ( new_state )
	{
		case DCOM_CLIENT_INTERNAL_PORT_DEVICE_DISCOVERED:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_NEW_DEVICE_DISCOVERED;
			break;

		case DCOM_CLIENT_INTERNAL_UNKNOWN_COMMAND:
		case DCOM_CLIENT_INTERNAL_FRAMING_ERROR:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_COMM_FAULT;
			break;

		case DCOM_CLIENT_INTERNAL_START_IDENTITY_EXCHANGE:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_PERFORMING_IDENTITY_EXCHANGE;
			break;

		case DCOM_CLIENT_INTERNAL_IDENTITY_SERVER_VALIDATION_FAILURE:
		case DCOM_CLIENT_INTERNAL_IDENTITY_CLIENT_VALIDATION_FAILURE:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_IDENTITY_MISMATCH;
			break;

		case DCOM_CLIENT_INTERNAL_START_MANAGE_SUBSCRIPTIONS:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_STARTING_MANAGE_SUBSCRIPTIONS;
			break;

		case DCOM_CLIENT_INTERNAL_START_SYNC_PARAMETERS:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_STARTING_PARAMETER_SYNC;
			break;

		case DCOM_CLIENT_INTERNAL_START_EXCHANGE_DELTA_DATA:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_CONNECTION_COMPLETE_DEVICE_ACTIVE;
			break;

		case DCOM_CLIENT_INTERNAL_DEVICE_REMOVE:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_REMOVED_OR_LOST;
			break;

		case DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_RESENT:
			dcom_device->m_dev_state = DCOM_CLIENT_DEV_MISSED_FRAME_RETRANSMITTING;
			break;

		case DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_FAILURE_RESPONSE:
		case DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_FAILURE_RESPONSE:
		case DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_FAILURE_RESPONSE:
		case DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_SUCCESS_RESPONSE:
		case DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_REQUEST_SENT:
		case DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_SUCCESS_RESPONSE:
		case DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_REQUEST_SENT:
		case DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_SUCCESS_RESPONSE:
		case DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_REQUEST_SENT:
		case DCOM_CLIENT_INTERNAL_IDENTITY_MANAGER_VALIDATION_FAILURE:
		case DCOM_CLIENT_INTERNAL_IDENTITY_SERVER_VALIDATION_SUCCESS:
		case DCOM_CLIENT_INTERNAL_IDENTITY_REQUEST_SENT:
		case DCOM_CLIENT_INTERNAL_NON_EXISTENT:
		default:
			send_update = false;
			break;
	}

	if ( send_update )
	{
		m_delta_com_manager->Delta_Com_Client_Dev_State_Change( m_client_id,
																dcom_device->m_dcom_dev_address,
																dcom_device->m_dev_state );
	}
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'past_state' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Client::Delta_Com_Response( uint8_t* data, uint16_t length,
										   delta_com_device_address dcom_dev_address )
{
	Dcom_Client_Device* dcom_device = Find_Dcom_Device( dcom_dev_address );
	Delta_Com::cmd_type_t command_type = Delta_Com::Get_Command_Type( data[COMMAND_BYTE] );
	uint16_t frame_length;
	bool_t is_it_fragmented;

	if ( nullptr != dcom_device )
	{
		frame_length = Data_Align<uint16_t>::From_Array( &data[LENGTH_LOW] );
		if ( frame_length == length )
		{
			switch ( Delta_Com::Get_Command( data[COMMAND_BYTE] ) )
			{
				case Delta_Com::EXCHANGE_DELTA_PARAMETERS:
					command_type = Exchange_Delta_Parameters_Response_Handler( dcom_device, data );
					if ( Delta_Com::SUCCESSFUL_RESPONSE == command_type )
					{
						if ( DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_REQUEST_SENT == dcom_device->m_dcom_internal_state )
						{
							dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_SUCCESS_RESPONSE;
							is_it_fragmented = Delta_Com::Is_Fragmented( data[COMMAND_BYTE] );
							if ( is_it_fragmented )
							{
								dcom_device->m_sync_completed = false;
							}
							else
							{
								dcom_device->m_sync_completed = true;
							}
						}
						else
						{
							dcom_device->m_dcom_internal_state =
								DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_SUCCESS_RESPONSE;
						}
					}
					else
					{
						if ( DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_REQUEST_SENT == dcom_device->m_dcom_internal_state )
						{
							dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_SYNC_PARAMETERS_FAILURE_RESPONSE;
						}
						else
						{
							dcom_device->m_dcom_internal_state =
								DCOM_CLIENT_INTERNAL_EXCHANGE_DELTA_DATA_FAILURE_RESPONSE;
						}
					}
					break;

				case Delta_Com::MANAGE_SUBSCRIPTIONS:
					if ( ( Delta_Com::SUCCESSFUL_RESPONSE == command_type ) ||
						 ( Delta_Com::PARAMETER_MISMATCH == command_type ) )	// If there is an unknown parameter, we
																				// just ignore for now
					{
						if ( Manage_Subscriptions_Response_Handler( dcom_device, data ) )
						{
							dcom_device->m_remote_sync_key = Delta_Com::INITIAL_SYNC_KEY;
							dcom_device->m_local_sync_key = 0U;
							dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_START_SYNC_PARAMETERS;
						}
						else
						{
							dcom_device->m_dcom_internal_state =
								DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_SUCCESS_RESPONSE;
						}
					}
					else
					{
						dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_MANAGE_SUBSCRIPTIONS_FAILURE_RESPONSE;
					}
					break;

				case Delta_Com::EXCHANGE_IDENTITY:
					Identity_Exchange_Response_Handler( dcom_device, data );
					if ( Delta_Com::SUCCESSFUL_RESPONSE == command_type )
					{
						dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_IDENTITY_SERVER_VALIDATION_SUCCESS;
					}
					else
					{
						dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_IDENTITY_SERVER_VALIDATION_FAILURE;
					}
					break;

				default:
					dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_UNKNOWN_COMMAND;
					break;
			}
		}
		else
		{
			dcom_device->m_dcom_internal_state = DCOM_CLIENT_INTERNAL_FRAMING_ERROR;
		}

		// We need to release the TX buffer as a response was received.
		m_delta_com_port->Release_TX_Buffer( dcom_dev_address );
		dcom_device->m_request_ready = false;
	}
	else
	{
		// Unexpected data, discard the frame
	}
}
