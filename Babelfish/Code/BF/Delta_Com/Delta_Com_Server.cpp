/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Delta_Com_Server.h"
#include "Data_Align.hpp"
#include "Delta_Com.h"
#include "Timers_Lib.h"
#include "DCI_Owner.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Server::Delta_Com_Server( Delta_Data::data_def_t const* delta_com_def,
									delta_com_identity_source_t const* identity_def,
									Delta_Com_Display* display, cback_func_t state_change_callback,
									cback_client_validate_func_t client_validate_callback,
									cback_param_t parameter ) :
	m_state( NO_COMMUNICATION ),
	m_display( reinterpret_cast<Delta_Com_Display*>( nullptr ) ),
	m_timer( new BF_Lib::Timers( &Comm_Loss_Static, this,
								 BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "DCOM Server" ) ),
	m_max_message_size( 0U ),
	m_deque_key( Delta_Com::INITIAL_SYNC_KEY ),
	m_prev_deque_key( Delta_Com::INITIAL_SYNC_KEY ),
	m_state_change_cback( state_change_callback ),
	m_client_validate_cback( client_validate_callback ),
	m_cback_param( parameter ),
	m_communication_timeout( DEFAULT_COMM_TIMEOUT ),
	m_delta_data( new Delta_Data( delta_com_def ) ),
	m_identity_def( identity_def )
{
	if ( m_identity_def != reinterpret_cast<delta_com_identity_source_t const*>( nullptr ) )
	{
		m_max_message_size = m_identity_def->m_buffer_size;
	}

	if ( display == reinterpret_cast<Delta_Com_Display*>( nullptr ) )
	{
		m_display = new Delta_Com_Display_Null();

	}
	else
	{
		m_display = display;
	}

	m_display->Idle();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Server::~Delta_Com_Server()
{
	delete m_display;
	delete m_timer;

	m_cback_param = nullptr;
	delete m_delta_data;
	m_identity_def = reinterpret_cast<delta_com_identity_source_t const*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Server::Communication_Loss_Handler( void )
{
	Initialize_Sub_Pub();

	m_state = COMMUNICATION_LOSS;

	if ( m_state_change_cback != reinterpret_cast<cback_func_t>( nullptr ) )
	{
		( *m_state_change_cback )( m_cback_param, COMMUNICATION_LOSS );
	}

	m_display->Faulted();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Server::Initialize_Sub_Pub( void )
{
	m_deque_key = Delta_Com::INITIAL_SYNC_KEY;
	m_delta_data->Reset_Full();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Com_Server::Process_Delta_Com_Message( uint8_t* rx_data, uint16_t rx_data_len,
												  uint8_t* tx_data, uint16_t* tx_data_len )
{
	delta_com_error_t status = UNKNOWN_ERROR;
	bool process_success = false;
	uint16_t command_length;

	Delta_Com::cmd_t command;

	command_length = Data_Align<uint16_t>::From_Array( &rx_data[LENGTH_LOW] );

	if ( command_length == rx_data_len )
	{
		command = Delta_Com::Get_Command( rx_data[COMMAND_BYTE] );

		tx_data[COMMAND_BYTE] = command;
		switch ( command )
		{
			case Delta_Com::EXCHANGE_DELTA_PARAMETERS:	// 0x05
				if ( 0U != m_communication_timeout )
				{
					m_timer->Start( m_communication_timeout );
				}
				status = Exchange_Delta_Parameters( rx_data, rx_data_len, tx_data, tx_data_len );
				break;

			case Delta_Com::MANAGE_SUBSCRIPTIONS:	// 0x06
				if ( 0U != m_communication_timeout )
				{
					m_timer->Start( m_communication_timeout );
				}
				status = Manage_Subscriptions( rx_data, rx_data_len, tx_data, tx_data_len );
				break;

			case Delta_Com::EXCHANGE_IDENTITY:	// 0x07
				status = Exchange_Identity( rx_data, rx_data_len, tx_data, tx_data_len );
				break;

			default:
				status = INVALID_COMMAND;
				break;
		}

		Data_Align<uint16_t>::To_Array( *tx_data_len, &tx_data[LENGTH_LOW] );

		if ( status != NO_DCOM_ERROR )
		{
			tx_data[COMMAND_BYTE] = Delta_Com::Set_Command_Type( tx_data[COMMAND_BYTE],
																 Delta_Com::ERROR_COMMAND );
			tx_data[ERROR_CODE] = status;
			Data_Align<uint16_t>::To_Array( ERROR_CODE_FRAME_LENGTH, &tx_data[LENGTH_LOW] );
			*tx_data_len = ERROR_CODE_FRAME_LENGTH;
		}

		process_success = true;
	}
	return ( process_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Server::Set_Maximum_Message_Size( uint16_t max_size )
{
	m_max_message_size = max_size;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Com_Server::Exchange_Delta_Parameters( uint8_t* rx_data,
															   uint16_t rx_data_len,
															   uint8_t* tx_data,
															   uint16_t* tx_data_len )
{
	delta_com_error_t return_status = NO_DCOM_ERROR;
	bool is_it_fragmented;
	bool server_in_valid_state;
	uint16_t tx_length_used;

	tx_data[COMMAND_BYTE] = Delta_Com::Set_Command( tx_data[COMMAND_BYTE],
													Delta_Com::EXCHANGE_DELTA_PARAMETERS );

	switch ( m_state )
	{
		case INITIALIZING:
		case INITIALIZED:
			server_in_valid_state = true;
			break;

		case NO_COMMUNICATION:
		case COMMUNICATION_LOSS:
		default:
			server_in_valid_state = false;
			break;
	}

	if ( ( rx_data[SET_DELTA_PREVIOUS_DE_QUE_KEY] == m_deque_key ) &&
		 ( server_in_valid_state == true ) )
	{
		// Save the key to check for retransmits
		m_prev_deque_key = m_deque_key;

		// Increment our own key and skip the initial.
		m_deque_key = m_deque_key + 1U;
		if ( m_deque_key == Delta_Com::INITIAL_SYNC_KEY )
		{
			m_deque_key = 0U;
		}

		// Pass the requesters dequeue key back.
		tx_data[GET_DELTA_PREVIOUS_DE_QUE_KEY] = rx_data[SET_DELTA_NEW_DE_QUE_KEY];
		tx_data[GET_DELTA_NEW_DE_QUE_KEY] = m_deque_key;

		return_status = m_delta_data->Set_Delta_Data( &rx_data[SET_DELTA_FRAME_BEGIN],
													  ( rx_data_len - SET_DELTA_FRAME_BEGIN ) );

		if ( return_status == NO_DCOM_ERROR )
		{
			tx_length_used = 0U;
			return_status = m_delta_data->Get_Delta_Data( &tx_data[GET_DELTA_FRAME_BEGIN],
														  &tx_length_used,
														  ( m_max_message_size -
															GET_DELTA_FRAME_BEGIN ) );

			*tx_data_len = tx_length_used + GET_DELTA_FRAME_BEGIN;

			if ( return_status == NO_DCOM_ERROR )
			{
				tx_data[COMMAND_BYTE] = Delta_Com::Set_Command_Type(
					tx_data[COMMAND_BYTE], Delta_Com::SUCCESSFUL_RESPONSE );
			}
		}
	}
	// This is indicative of a client re-transmitting the message.  It appears we use the exact
	// same buffer as before.  Just simply retransmit.
	else if ( ( rx_data[SET_DELTA_PREVIOUS_DE_QUE_KEY] == m_prev_deque_key ) &&
			  ( server_in_valid_state == true ) )
	{
		*tx_data_len = Data_Align<uint16_t>::From_Array( &tx_data[LENGTH_LOW] );
		tx_data[COMMAND_BYTE] = Delta_Com::Set_Command_Type(
			tx_data[COMMAND_BYTE], Delta_Com::SUCCESSFUL_RESPONSE );
		return_status = NO_DCOM_ERROR;
	}
	else// This is an error state
	{
		return_status = SYNCHRONIZATION_ERROR;
	}

	if ( m_state != INITIALIZED )
	{
		is_it_fragmented = Delta_Com::Is_Fragmented( rx_data[COMMAND_BYTE] );
		if ( m_delta_data->Sync_Complete() && ( is_it_fragmented == false ) )
		{
			m_state = INITIALIZED;
			m_display->Active();
			if ( m_state_change_cback != nullptr )
			{
				( *m_state_change_cback )( m_cback_param, m_state );
			}
		}
	}

	if ( !m_delta_data->Sync_Complete() )
	{
		tx_data[COMMAND_BYTE] =
			Delta_Com::Set_Fragmented( tx_data[COMMAND_BYTE] );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Com_Server::Manage_Subscriptions( uint8_t* rx_data,
														  uint16_t rx_data_len, uint8_t* tx_data,
														  uint16_t* tx_data_len )
{
	uint16_t rx_data_itr;
	uint16_t number_of_parameters;
	uint8_t tracking_type;
	uint16_t remaining_len;
	uint16_t parameters_left;
	uint16_t tx_length_used = 0U;

	m_deque_key = Delta_Com::INITIAL_SYNC_KEY;

	rx_data_itr = MANAGE_SUBSCRIPTIONS_FRAME_BEGIN;
	// First we handle the publish requests.
	while ( rx_data_itr < rx_data_len )
	{
		tracking_type = rx_data[rx_data_itr];
		rx_data_itr += ATTRIBUTE_LENGTH;
		number_of_parameters = Data_Align<uint16_t>::From_Array( &rx_data[rx_data_itr] );
		rx_data_itr += COUNT_LENGTH;

		m_delta_data->Publish_Request( &rx_data[rx_data_itr], number_of_parameters, tracking_type );

		rx_data_itr += static_cast<uint16_t>( PARAMETER_LENGTH * number_of_parameters );
	}

	// Then we handle the Subscription Requests.
	tx_data[COMMAND_BYTE] = Delta_Com::Set_Command(
		tx_data[COMMAND_BYTE], Delta_Com::MANAGE_SUBSCRIPTIONS );
	tx_data[COMMAND_BYTE] = Delta_Com::Set_Command_Type(
		tx_data[COMMAND_BYTE], Delta_Com::SUCCESSFUL_RESPONSE );

	tx_data[MANAGE_SUBSCRIPTIONS_RETURN_SUBSCRIPTION_TYPE] = SERVER_PUB_REQ;

	remaining_len = m_max_message_size - MANAGE_SUBSCRIPTIONS_RETURN_SUBSCRIPTION_TYPE;
	parameters_left = m_delta_data->Subscription_Req_List(
		&tx_data[MANAGE_SUBSCRIPTIONS_RETURN_SUBSCRIPTION_TYPE], &tx_length_used,
		remaining_len, SERVER_PUB_REQ );

	*tx_data_len = tx_length_used + MANAGE_SUBSCRIPTIONS_RETURN_SUBSCRIPTION_TYPE;

	if ( parameters_left > 0U )
	{
		tx_data[COMMAND_BYTE] =
			Delta_Com::Set_Fragmented( tx_data[COMMAND_BYTE] );
	}

	m_state = INITIALIZING;

	return ( NO_DCOM_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Com_Server::Exchange_Identity( uint8_t* rx_data,
													   uint16_t rx_data_len, uint8_t* tx_data,
													   uint16_t* tx_data_len )
{
	uint16_t their_buffer_size;
	uint16_t our_buffer_size;
	uint16_t their_update_interval;
	uint16_t our_update_interval;
	uint32_t timeout_calc;
	delta_com_identity_t client_identity;
	dcom_identity_error identity_check_status;

	m_display->Discovery();
	m_state = INITIALIZING;

	Initialize_Sub_Pub();

	if ( m_identity_def == reinterpret_cast<delta_com_identity_source_t const*>( nullptr ) )
	{
		BF_Lib::Copy_Val_To_String( &tx_data[IDENTITY_REQUEST_FRAME_BEGIN],
									Delta_Com::UNDEFINED_IDENTITY_DATA,
									( IDENTITY_REQUEST_FRAME_LENGTH -
									  IDENTITY_REQUEST_FRAME_BEGIN ) );
	}
	else
	{
		client_identity.m_identity_revision =
			rx_data[IDENTITY_REQUEST_FRAME_ID_BLOCK_COMPATIBILITY];
		client_identity.m_product_compatibility_number =
			rx_data[IDENTITY_REQUEST_FRAME_PROD_COMPATIBILITY];
		client_identity.m_product_code = Data_Align<uint16_t>
			::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_PROD_CODE_0] );
		client_identity.m_product_subcode = Data_Align<uint16_t>
			::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_SUB_PROD_CODE_0] );
		client_identity.m_firmware_revision = Data_Align<uint32_t>
			::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_FW_REV_0] );
		client_identity.m_hardware_revision = Data_Align<uint16_t>
			::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_HW_REV_0] );
		client_identity.m_device_serial_number = Data_Align<uint32_t>
			::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_SERIAL_NUM_0] );
		client_identity.m_buffer_size = Data_Align<uint16_t>
			::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_BUFFER_SIZE_0] );
		client_identity.m_update_interval_time = Data_Align<uint16_t>
			::From_Array( &rx_data[IDENTITY_REQUEST_FRAME_UPDATE_INTERVAL_TIME_0] );

		if ( client_identity.m_identity_revision == m_identity_def->m_identity_revision )
		{
			our_buffer_size = m_identity_def->m_buffer_size;
			our_update_interval = m_identity_def->m_update_interval_time;

			if ( rx_data_len == IDENTITY_REQUEST_FRAME_LENGTH )
			{
				their_buffer_size = Data_Align<uint16_t>::From_Array(
					&rx_data[IDENTITY_REQUEST_FRAME_BUFFER_SIZE_0] );
				their_update_interval = Data_Align<uint16_t>::From_Array(
					&rx_data[IDENTITY_REQUEST_FRAME_UPDATE_INTERVAL_TIME_0] );

				if ( ( our_update_interval < their_update_interval ) ||
					 ( their_update_interval == 0U ) )
				{
					our_update_interval = their_update_interval;
				}
				if ( our_buffer_size > their_buffer_size )
				{
					our_buffer_size = their_buffer_size;
				}
			}

			tx_data[IDENTITY_REQUEST_FRAME_ID_BLOCK_COMPATIBILITY] =
				m_identity_def->m_identity_revision;
			tx_data[IDENTITY_REQUEST_FRAME_PROD_COMPATIBILITY] =
				m_identity_def->m_product_compatibility_number;

			Data_Align<uint16_t>::To_Array( *m_identity_def->m_product_code,
											&tx_data[IDENTITY_REQUEST_FRAME_PROD_CODE_0] );
			Data_Align<uint16_t>::To_Array( *m_identity_def->m_product_subcode,
											&tx_data[IDENTITY_REQUEST_FRAME_SUB_PROD_CODE_0] );
			Data_Align<uint32_t>::To_Array( *m_identity_def->m_firmware_revision,
											&tx_data[IDENTITY_REQUEST_FRAME_FW_REV_0] );
			Data_Align<uint16_t>::To_Array( *m_identity_def->m_hardware_revision,
											&tx_data[IDENTITY_REQUEST_FRAME_HW_REV_0] );
			Data_Align<uint32_t>::To_Array( *m_identity_def->m_device_serial_number,
											&tx_data[IDENTITY_REQUEST_FRAME_SERIAL_NUM_0] );

			Data_Align<uint16_t>::To_Array( our_buffer_size,
											&tx_data[IDENTITY_REQUEST_FRAME_BUFFER_SIZE_0] );
			Data_Align<uint16_t>::To_Array( our_update_interval,
											&tx_data[
												IDENTITY_REQUEST_FRAME_UPDATE_INTERVAL_TIME_0] );

			tx_data[COMMAND_BYTE] = Delta_Com::Set_Command_Type(
				tx_data[COMMAND_BYTE], Delta_Com::SUCCESSFUL_RESPONSE );

			m_max_message_size = our_buffer_size;

			timeout_calc = our_update_interval;
			timeout_calc = timeout_calc * COMMUNICATION_TIMEOUT_POLL_MULTIPLIER;
			if ( timeout_calc > MAX_TIMEOUT )
			{
				timeout_calc = static_cast<uint16_t>( MAX_TIMEOUT );
			}
			m_communication_timeout = static_cast<BF_Lib::Timers::TIMERS_TIME_TD>
				( timeout_calc );

			if ( m_client_validate_cback != nullptr )
			{
				identity_check_status =
					( *m_client_validate_cback )( m_cback_param, &client_identity );
				if ( DCOM_IDENTITY_ERROR_SUCCESS != identity_check_status )
				{
					tx_data[COMMAND_BYTE] = Delta_Com::Set_Command_Type(
						tx_data[COMMAND_BYTE], Delta_Com::ERROR_COMMAND );
				}
			}
		}
		else
		{
			tx_data[COMMAND_BYTE] = Delta_Com::Set_Command_Type(
				tx_data[COMMAND_BYTE], Delta_Com::ERROR_COMMAND );
		}
	}

	*tx_data_len = IDENTITY_REQUEST_FRAME_LENGTH;

	return ( NO_DCOM_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ACCESS_CMD_TD Delta_Com_Server::Translate_Attribute_To_DCI_Set_Command(
	uint8_t attribute )
{
	DCI_ACCESS_CMD_TD command = DCI_ACCESS_UNDEFINED_COMMAND;

	switch ( attribute )
	{
		case CURRENT_VALUE:
			command = DCI_ACCESS_SET_RAM_CMD;
			break;

		case INITIAL_VALUE:
			command = DCI_ACCESS_SET_INIT_CMD;
			break;

		case DEFAULT_VALUE:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;

		case LENGTH:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;

		case DATATYPE:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;

		case ATTRIBUTE_INFORMATION:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;

		case MIN:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;

		case MAX:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;

		case ENUM_LIST:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;

		default:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;
	}

	return ( command );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ACCESS_CMD_TD Delta_Com_Server::Translate_Attribute_To_DCI_Get_Command(
	uint8_t attribute )
{
	DCI_ACCESS_CMD_TD command = DCI_ACCESS_UNDEFINED_COMMAND;

	switch ( attribute )
	{
		case CURRENT_VALUE:
			command = DCI_ACCESS_GET_RAM_CMD;
			break;

		case INITIAL_VALUE:
			command = DCI_ACCESS_GET_INIT_CMD;
			break;

		case DEFAULT_VALUE:
			command = DCI_ACCESS_GET_DEFAULT_CMD;
			break;

		case LENGTH:
			command = DCI_ACCESS_GET_LENGTH_CMD;
			break;

		case ATTRIBUTE_INFORMATION:
			command = DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD;
			break;

		case MIN:
			command = DCI_ACCESS_GET_MIN_CMD;
			break;

		case MAX:
			command = DCI_ACCESS_GET_MAX_CMD;
			break;

		case ENUM_LIST_LENGTH:
			command = DCI_ACCESS_GET_ENUM_LIST_LEN_CMD;
			break;

		case ENUM_LIST:
			command = DCI_ACCESS_GET_ENUM_CMD;
			break;

		default:
			command = DCI_ACCESS_UNDEFINED_COMMAND;
			break;
	}

	return ( command );
}
