/*
 * Modbus_Delta_Com_Client_Port.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: E9924418
 */


#include "Modbus_Delta_Com_Client_Port.h"
#include "Modbus_Master_No_OS.h"
#include "Modbus_Negotiation.h"
#include "Ram.h"
#include "Includes.h"
#include "Data_Align.hpp"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Delta_Com_Client_Port::Modbus_Delta_Com_Client_Port( uint8_t max_devices,
															uint8_t max_active_devices,
															BF_Mbus::Modbus_Master_No_OS* modbus_master_no_os,
															uint32_t baud_rate,
															uint8_t num_buffers,
															bool_t auto_discovery_enabled )
	: m_num_devices( max_devices ),
	m_modbus_master_ctrl( modbus_master_no_os ),
	m_active_device( INVALID_DELTA_COM_DEVICE_ADDRESS ),
	m_dcom_client( reinterpret_cast<IDelta_Com_Client*>( nullptr ) ),
	m_max_active_devices( max_active_devices ),
	m_num_active_devices( 0U ),
	m_negotiation_baud_rate( Modbus_Negotiation::NEGOTIATION_BAUD_DEFAULT ),
	m_operational_baud_rate( baud_rate ),
	m_current_scan_index( 0U ),
	m_buffer_list( reinterpret_cast<Modbus_Port_Buffer*>( nullptr ) ),
	m_modbus_device_list( reinterpret_cast<Modbus_Port_Device*>( nullptr ) ),
	m_num_buffers( 0U ),
	m_negotiation_message_timeout( 0U ),
	m_operational_message_timeout( 0U ),
	m_current_scan_baud_rate( 0U ),
	m_scan_task( reinterpret_cast<CR_Tasker*>( nullptr ) )
{

	m_current_scan_baud_rate = m_negotiation_baud_rate;

	m_modbus_device_list = reinterpret_cast<Modbus_Port_Device*>(
		Ram::Allocate( sizeof( Modbus_Port_Device ) * max_devices ) );

	for ( uint8_t dev_index = 0U; dev_index < m_num_devices; dev_index++ )
	{
		m_modbus_device_list[dev_index].m_dev_state = DCOM_PORT_DEV_NON_EXISTENT;
		m_modbus_device_list[dev_index].m_modbus_address = MODBUS_INVALID_DEVICE_ADDRESS;
		m_modbus_device_list[dev_index].m_tx_buffer_index = INVALID_BUFFER_INDEX;
		m_modbus_device_list[dev_index].m_link_negotiation_state =
			LINK_NEGOTIATION_BAUD_REQUEST_SUCCESS;
		m_modbus_device_list[dev_index].m_auto_discovery_enabled = auto_discovery_enabled;
	}

	if ( ONE_PER_DEVICE == num_buffers )
	{
		m_num_buffers = m_num_devices;
	}
	else
	{
		m_num_buffers = num_buffers;
	}

	m_buffer_list = reinterpret_cast<Modbus_Port_Buffer*>(
		Ram::Allocate( sizeof( Modbus_Port_Buffer ) * m_num_buffers ) );

	for ( uint8_t buffer_index = 0U; buffer_index < m_num_buffers; buffer_index++ )
	{
		m_buffer_list[buffer_index].m_in_use = false;
	}

	m_scan_task = new CR_Tasker( &Device_Scan_Task_Static, this, CR_TASKER_IDLE_PRIORITY,
								 "Dcom Modbus Scan Task" );

	if ( !auto_discovery_enabled )
	{
		m_scan_task->Suspend();
	}

	m_modbus_master_ctrl->Attach_Callback( &Modbus_Callback_Static, this );
	m_modbus_master_ctrl->Disable_Port();

	m_negotiation_message_timeout =
		( TIMEOUT_MULTIPLIER * NEGOTIATION_FRAME_TIMEOUT_BYTES * BITS_IN_BYTE *
		  MILLISECONDS_IN_SECOND ) / m_negotiation_baud_rate;
	m_operational_message_timeout =
		( TIMEOUT_MULTIPLIER * MODBUS_MAX_RTU_MSG_LENGTH * BITS_IN_BYTE * MILLISECONDS_IN_SECOND ) /
		m_operational_baud_rate;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_port_status Modbus_Delta_Com_Client_Port::Add_Modbus_Device( uint8_t modbus_address )
{
	delta_com_port_status return_status = DCOM_PORT_SUCCESS;
	uint8_t dev_index;

	if ( MODBUS_INVALID_DEVICE_ADDRESS == modbus_address )
	{
		return_status = DCOM_PORT_FAILURE;
	}
	else
	{
		// Make sure the modbus address is not in the list of devices
		for ( dev_index = 0U; dev_index < m_num_devices; dev_index++ )
		{
			if ( ( DCOM_PORT_DEV_NON_EXISTENT != m_modbus_device_list[dev_index].m_dev_state ) &&
				 ( modbus_address == m_modbus_device_list[dev_index].m_modbus_address ) )
			{
				return_status = DCOM_PORT_FAILURE;
				break;
			}
		}

		if ( DCOM_PORT_SUCCESS == return_status )
		{
			// Find a free slot
			for ( dev_index = 0U; dev_index < m_num_devices; dev_index++ )
			{
				if ( DCOM_PORT_DEV_NON_EXISTENT == m_modbus_device_list[dev_index].m_dev_state )
				{
					m_modbus_device_list[dev_index].m_modbus_address = modbus_address;
					m_modbus_device_list[dev_index].m_dev_state =
						DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK;
					m_modbus_device_list[dev_index].m_auto_discovery_enabled = false;
					if ( m_num_active_devices < m_max_active_devices )
					{
						m_scan_task->Resume();
					}
					break;
				}
			}

			if ( dev_index >= m_num_devices )
			{
				return_status = DCOM_PORT_OUT_OF_RESOURCE;
			}
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Delta_Com_Client_Port::Delta_Com_Client_Set( IDelta_Com_Client* dcom_client )
{
	m_dcom_client = dcom_client;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_port_status Modbus_Delta_Com_Client_Port::Get_TX_Buffer( delta_com_device_address address,
																   uint8_t** buffer,
																   uint16_t** tx_length )
{
	delta_com_port_status return_status = DCOM_PORT_SUCCESS;
	uint8_t buffer_index = 0U;

	BF_ASSERT(Is_Dcom_Address_Valid( address ));

	if ( DCOM_PORT_DEV_NON_EXISTENT == m_modbus_device_list[address].m_dev_state )
	{
		return_status = DCOM_PORT_INVALID_DCOM_ADDRESS;
	}
	else if ( ( ( INVALID_DELTA_COM_DEVICE_ADDRESS != m_active_device ) &&
				( address != m_active_device ) ) ||
			  ( ( address == m_active_device ) &&
				( ( DCOM_PORT_DEV_IDLE != m_modbus_device_list[address].m_dev_state ) &&
				  ( DCOM_PORT_DEV_TIMEOUT !=
					m_modbus_device_list[address].m_dev_state ) &&
				  ( DCOM_PORT_DEV_ERROR !=
					m_modbus_device_list[address].m_dev_state ) ) ) )
	{
		return_status = DCOM_PORT_BUSY;
	}
	else if ( INVALID_BUFFER_INDEX != m_modbus_device_list[address].m_tx_buffer_index )
	{
		*buffer = m_buffer_list[m_modbus_device_list[address].m_tx_buffer_index].m_buffer;
		*tx_length = &m_buffer_list[m_modbus_device_list[address].m_tx_buffer_index].m_length;
	}
	else
	{
		for ( buffer_index = 0U; buffer_index < m_num_buffers; buffer_index++ )
		{
			if ( false == m_buffer_list[buffer_index].m_in_use )
			{
				break;
			}
		}
		if ( buffer_index >= m_num_buffers )
		{
			return_status = DCOM_PORT_OUT_OF_RESOURCE;
		}
		else
		{
			m_buffer_list[buffer_index].m_in_use = true;
			*buffer = m_buffer_list[buffer_index].m_buffer;
			*tx_length = &m_buffer_list[buffer_index].m_length;
			m_modbus_device_list[address].m_tx_buffer_index = buffer_index;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Delta_Com_Client_Port::Release_TX_Buffer( delta_com_device_address address )
{
	BF_ASSERT( Is_Dcom_Address_Valid( address ) );

	if ( INVALID_BUFFER_INDEX != m_modbus_device_list[address].m_tx_buffer_index )
	{
		m_buffer_list[m_modbus_device_list[address].m_tx_buffer_index].m_in_use = false;
		m_modbus_device_list[address].m_tx_buffer_index = INVALID_BUFFER_INDEX;
		if ( address == m_active_device )
		{
			m_active_device = INVALID_DELTA_COM_DEVICE_ADDRESS;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Delta_Com_Client_Port::Process_Link_Message( uint8_t* data, uint16_t length,
														 uint8_t error )
{
	if ( LINK_NEGOTIATION_BAUD_REQUEST_SENT ==
		 m_modbus_device_list[m_current_scan_index].m_link_negotiation_state )
	{
		if ( MB_NO_ERROR_CODE == error )
		{
			m_modbus_device_list[m_current_scan_index].m_link_negotiation_state =
				LINK_NEGOTIATION_BAUD_REQUEST_SUCCESS;
		}
		else
		{
			m_modbus_device_list[m_current_scan_index].m_link_negotiation_state =
				LINK_NEGOTIATION_BAUD_REQUEST_TIMEDOUT_OR_ERROR;
		}
	}
	else if ( LINK_NEGOTIATION_SET_MODBUS_ADDR_SENT ==
			  m_modbus_device_list[m_current_scan_index].m_link_negotiation_state )
	{
		if ( MB_NO_ERROR_CODE == error )
		{
			m_modbus_device_list[m_current_scan_index].m_link_negotiation_state =
				LINK_NEGOTIATION_SET_MODBUS_ADDR_SUCCESS;
		}
		else
		{
			m_modbus_device_list[m_current_scan_index].m_link_negotiation_state =
				LINK_NEGOTIATION_SET_MODBUS_ADDR_TIMEOUT_OR_ERROR;
		}
	}
	else
	{
		// do nothing
	}
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'length' not referenced
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
void Modbus_Delta_Com_Client_Port::Device_Scan_Task()
{
	bool_t restore_baud = false;
	bool_t move_to_next = false;
	bool_t free_the_port = false;
	bool_t claim_the_port = false;
	bool_t free_buffer = false;
	uint8_t buffer_index = 0U;

	if ( m_num_active_devices >= m_max_active_devices )
	{
		m_scan_task->Suspend_And_Reset();
	}
	else if ( ( INVALID_DELTA_COM_DEVICE_ADDRESS == m_active_device ) ||
			  ( m_current_scan_index == m_active_device ) )
	{
		switch ( m_modbus_device_list[m_current_scan_index].m_dev_state )
		{
			case DCOM_PORT_DEV_NON_EXISTENT:
				if ( m_modbus_device_list[m_current_scan_index].m_auto_discovery_enabled )
				{
					m_modbus_device_list[m_current_scan_index].m_dev_state =
						DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK;
					m_modbus_device_list[m_current_scan_index].m_modbus_address =
						MODBUS_INVALID_DEVICE_ADDRESS;
					claim_the_port = true;
				}
				else
				{
					// We should not get here
					move_to_next = true;
				}
				break;

			case DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK:
				if ( MODBUS_INVALID_DEVICE_ADDRESS ==
					 m_modbus_device_list[m_current_scan_index].m_modbus_address )
				{
					// wiggle the address/attention lines as necessary
					// Get a buffer
					// Open the port with the current scan baud rate.
					// Frame the set address message
					// claim the port
					// Send the message
					// m_modbus_device_list[m_current_scan_index].m_dev_state =
					// DCOM_PORT_DEV_ESTABLISHING_LINK;
					// m_modbus_device_list[m_current_scan_index].m_link_negotiation_state =
					// LINK_NEGOTIATION_SET_MODBUS_ADDR_SENT;
				}
				else// A valid modbus address is already available
				{
					m_modbus_device_list[m_current_scan_index].m_dev_state =
						DCOM_PORT_DEV_ESTABLISHING_LINK;
					m_modbus_device_list[m_current_scan_index].m_link_negotiation_state =
						LINK_NEGOTIATION_SET_MODBUS_ADDR_SUCCESS;
				}
				break;

			case DCOM_PORT_DEV_ESTABLISHING_LINK:
				if ( LINK_NEGOTIATION_SET_MODBUS_ADDR_SENT ==
					 m_modbus_device_list[m_current_scan_index].m_link_negotiation_state )
				{
					// Wait till we get a success or error/timeout response
				}
				else if ( LINK_NEGOTIATION_SET_MODBUS_ADDR_SUCCESS ==
						  m_modbus_device_list[m_current_scan_index].m_link_negotiation_state )
				{
					// First find a free buffer
					for ( buffer_index = 0U; buffer_index < m_num_buffers; buffer_index++ )
					{
						if ( false == m_buffer_list[buffer_index].m_in_use )
						{
							break;
						}
					}
					if ( buffer_index < m_num_buffers )
					{
						m_buffer_list[buffer_index].m_in_use = true;
						m_modbus_device_list[m_current_scan_index].m_tx_buffer_index = buffer_index;

						Re_Enable_Port( m_current_scan_baud_rate );

						// Frame the set baud rate request
						m_buffer_list[buffer_index].m_buffer[Modbus_Negotiation::TX_MODE] =
							Modbus_Negotiation::TX_Mode( DCOM_MODBUS_TX_MODE );
						m_buffer_list[buffer_index].m_buffer[Modbus_Negotiation::PARITY] =
							Modbus_Negotiation::Parity( DCOM_MODBUS_PARITY_MODE );
						Data_Align<uint32_t>
						::To_Array( m_operational_baud_rate,
									&m_buffer_list[buffer_index].m_buffer[Modbus_Negotiation::
																		  BAUD_RATE] );
						m_buffer_list[buffer_index].m_buffer[Modbus_Negotiation::USE_LONG_HOLDOFF] =
							Modbus_Negotiation::Long_Holdoff( DCOM_MODBUS_LONG_HOLDOFF );
						m_buffer_list[buffer_index].m_buffer[Modbus_Negotiation::STOP_BIT] =
							Modbus_Negotiation::Stop_Bits( DCOM_MODBUS_STOP_BITS );

						// Send the set baud rate request
						m_modbus_master_ctrl->User_Func_Code( m_modbus_device_list[
																  m_current_scan_index].m_modbus_address,
															  Modbus_Negotiation::MODBUS_NEGOTIATION_CODE,
															  m_buffer_list[buffer_index].m_buffer,
															  Modbus_Negotiation::NEGOTIATION_FRAME_LENGTH, m_rx_buffer,
															  m_max_message_size );

						m_modbus_device_list[m_current_scan_index].m_link_negotiation_state =
							LINK_NEGOTIATION_BAUD_REQUEST_SENT;
						claim_the_port = true;
					}
				}
				else if ( LINK_NEGOTIATION_SET_MODBUS_ADDR_TIMEOUT_OR_ERROR ==
						  m_modbus_device_list[m_current_scan_index].m_link_negotiation_state )
				{
					m_modbus_device_list[m_current_scan_index].m_dev_state =
						DCOM_PORT_DEV_NON_EXISTENT;
					restore_baud = true;
					move_to_next = true;
					free_the_port = true;
					free_buffer = true;
				}
				else
				{
					// do nothing
				}

				if ( LINK_NEGOTIATION_BAUD_REQUEST_SENT ==
					 m_modbus_device_list[m_current_scan_index].m_link_negotiation_state )
				{
					// Wait till we get a successful response or until we timeout
				}
				else if ( LINK_NEGOTIATION_BAUD_REQUEST_TIMEDOUT_OR_ERROR ==
						  m_modbus_device_list[m_current_scan_index].m_link_negotiation_state )
				{
					m_modbus_device_list[m_current_scan_index].m_dev_state =
						DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK;
					restore_baud = true;
					move_to_next = true;
					free_the_port = true;
					free_buffer = true;
				}
				else if ( LINK_NEGOTIATION_BAUD_REQUEST_SUCCESS ==
						  m_modbus_device_list[m_current_scan_index].m_link_negotiation_state )
				{
					if ( reinterpret_cast<IDelta_Com_Client*>( nullptr ) != m_dcom_client )
					{
						m_modbus_device_list[m_current_scan_index].m_dev_state = DCOM_PORT_DEV_IDLE;
						m_num_active_devices = m_num_active_devices + 1U;
						if ( DCOM_CLIENT_SUCCESS !=
							 m_dcom_client->Delta_Com_Port_Dev_State_Change( m_current_scan_index,
																			 DCOM_PORT_DEV_IDLE ) )
						{	// Putting it back to re-negotiation because client was not ready to accept
							// the new device
							m_modbus_device_list[m_current_scan_index].m_dev_state =
								DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK;
						}
					}
					else// Putting it back to re-negotiation because client was not ready to accept
						// the new device
					{
						m_modbus_device_list[m_current_scan_index].m_dev_state =
							DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK;
					}
					restore_baud = true;
					move_to_next = true;
					free_the_port = true;
					free_buffer = true;
				}
				else
				{
					// do nothing
				}
				break;

			default:
				// any other state, we just want to move the scan iterator
				move_to_next = true;
		}
	}
	else
	{
		// do nothing
	}
	if ( free_buffer )
	{
		m_buffer_list[buffer_index].m_in_use = false;
		m_modbus_device_list[m_current_scan_index].m_tx_buffer_index = INVALID_BUFFER_INDEX;
	}

	if ( restore_baud && ( m_current_scan_baud_rate != m_operational_baud_rate ) )
	{
		Re_Enable_Port( m_operational_baud_rate );
	}

	if ( claim_the_port )
	{
		m_active_device = m_current_scan_index;
	}

	if ( free_the_port )
	{
		m_active_device = INVALID_DELTA_COM_DEVICE_ADDRESS;
	}

	if ( move_to_next )
	{
		m_current_scan_index = m_current_scan_index + 1U;
		if ( m_current_scan_index >= m_num_devices )
		{
			m_current_scan_index = 0U;
			if ( m_current_scan_baud_rate == m_operational_baud_rate )
			{
				m_current_scan_baud_rate = m_negotiation_baud_rate;
			}
			else
			{
				m_current_scan_baud_rate = m_operational_baud_rate;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Delta_Com_Client_Port::Re_Enable_Port( uint32_t baud )
{
	uint32_t timeout;

	if ( m_operational_baud_rate == baud )
	{
		timeout = m_operational_message_timeout;
	}
	else
	{
		timeout = m_negotiation_message_timeout;
	}
	m_modbus_master_ctrl->Disable_Port();
	m_modbus_master_ctrl->Enable_Port( DCOM_MODBUS_TX_MODE, DCOM_MODBUS_PARITY_MODE, baud,
									   DCOM_MODBUS_LONG_HOLDOFF, DCOM_MODBUS_STOP_BITS );
	m_modbus_master_ctrl->Set_Timeout( timeout );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Delta_Com_Client_Port::Disable_Port()
{
	m_scan_task->Suspend();
	m_modbus_master_ctrl->Disable_Port();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Modbus_Delta_Com_Client_Port::Max_Payload_Transmit_Time_Ms()
{
	return ( ( MODBUS_MAX_RTU_MSG_LENGTH * BITS_IN_BYTE * MILLISECONDS_IN_SECOND ) /
			 m_operational_baud_rate );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_port_status Modbus_Delta_Com_Client_Port::Send_Data( delta_com_device_address address )
{
	delta_com_port_status return_status = DCOM_PORT_SUCCESS;

	if ( !Is_Dcom_Address_Valid( address ) )
	{
		return_status = DCOM_PORT_INVALID_DCOM_ADDRESS;
	}
	else if ( DCOM_PORT_DEV_NON_EXISTENT == m_modbus_device_list[address].m_dev_state )
	{
		return_status = DCOM_PORT_INVALID_DCOM_ADDRESS;
	}
	else if ( ( ( INVALID_DELTA_COM_DEVICE_ADDRESS != m_active_device ) &&
				( address != m_active_device ) ) ||
			  ( ( address == m_active_device ) &&
				( ( DCOM_PORT_DEV_IDLE != m_modbus_device_list[address].m_dev_state ) &&
				  ( DCOM_PORT_DEV_TIMEOUT !=
					m_modbus_device_list[address].m_dev_state ) &&
				  ( DCOM_PORT_DEV_ERROR !=
					m_modbus_device_list[address].m_dev_state ) ) ) )
	{
		return_status = DCOM_PORT_BUSY;
	}
	else
	{
		m_modbus_master_ctrl->User_Func_Code( m_modbus_device_list[address].m_modbus_address,
											  DELTA_COM_FUNCTION_CODE,
											  m_buffer_list[m_modbus_device_list[address].
															m_tx_buffer_index].m_buffer,
											  m_buffer_list[m_modbus_device_list[address].
															m_tx_buffer_index].m_length,
											  m_rx_buffer, m_max_message_size );

		m_modbus_device_list[address].m_dev_state = DCOM_PORT_DEV_TRANSACTING;
		m_active_device = address;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Delta_Com_Client_Port::Modbus_Callback( uint8_t* data, uint16_t length,
													uint8_t mbus_return_status )
{
	bool_t send_state_change = false;
	delta_com_device_address dcom_dev_address = m_active_device;

	if ( INVALID_DELTA_COM_DEVICE_ADDRESS == m_active_device )
	{
		// Drop any frames that we are not expecting
	}
	else
	{
		if ( DCOM_PORT_DEV_ESTABLISHING_LINK == m_modbus_device_list[m_active_device].m_dev_state )
		{
			Process_Link_Message( data, length, mbus_return_status );
		}
		else
		{
			if ( MB_NO_ERROR_CODE == mbus_return_status )
			{
				m_modbus_device_list[m_active_device].m_dev_state = DCOM_PORT_DEV_IDLE;
				m_active_device = INVALID_DELTA_COM_DEVICE_ADDRESS;
				m_dcom_client->Delta_Com_Response( data, length, dcom_dev_address );
			}
			else if ( MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE == mbus_return_status )
			{
				m_modbus_device_list[m_active_device].m_dev_state = DCOM_PORT_DEV_TIMEOUT;
			}
			else
			{
				m_modbus_device_list[m_active_device].m_dev_state = DCOM_PORT_DEV_ERROR;
			}

			send_state_change = true;
		}
	}

	if ( send_state_change )
	{
		m_dcom_client->Delta_Com_Port_Dev_State_Change( dcom_dev_address,
														m_modbus_device_list[dcom_dev_address].m_dev_state );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Delta_Com_Client_Port::Remove_Device( delta_com_device_address address )
{
	bool_t free_buffer = false;

	if ( Is_Dcom_Address_Valid( address ) )
	{
		if ( m_modbus_device_list[address].m_auto_discovery_enabled )
		{
			if ( DCOM_PORT_DEV_NON_EXISTENT != m_modbus_device_list[address].m_dev_state )
			{
				m_modbus_device_list[address].m_dev_state = DCOM_PORT_DEV_NON_EXISTENT;
				m_num_active_devices = m_num_active_devices - 1U;
				m_active_device = INVALID_DELTA_COM_DEVICE_ADDRESS;
				free_buffer = true;
			}
		}
		else
		{
			if ( DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK !=
				 m_modbus_device_list[address].m_dev_state )
			{
				m_modbus_device_list[address].m_dev_state = DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK;
				m_num_active_devices = m_num_active_devices - 1U;
				m_active_device = INVALID_DELTA_COM_DEVICE_ADDRESS;
				free_buffer = true;
			}
		}

		if ( free_buffer )
		{
			if ( INVALID_BUFFER_INDEX != m_modbus_device_list[address].m_tx_buffer_index )
			{
				m_buffer_list[m_modbus_device_list[address].m_tx_buffer_index].m_in_use = false;
			}
			m_modbus_device_list[address].m_tx_buffer_index = INVALID_BUFFER_INDEX;
		}

		if ( m_num_active_devices < m_max_active_devices )
		{
			m_scan_task->Resume();
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Modbus_Delta_Com_Client_Port::Is_Dcom_Address_Valid( delta_com_device_address address ) const
{
	bool_t valid;

	if ( address < m_num_devices )
	{
		valid = true;
	}
	else
	{
		valid = false;
	}
	return ( valid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Modbus_Delta_Com_Client_Port::Max_Payload_Size_Get()
{
	return ( m_max_message_size );
}
