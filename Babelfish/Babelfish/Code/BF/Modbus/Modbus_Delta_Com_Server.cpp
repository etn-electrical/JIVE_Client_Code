/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Modbus_Delta_Com_Server.h"
#include "Modbus_Negotiation.h"
#include "Exception.h"

namespace BF_Mbus
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Delta_Com_Server::Modbus_Delta_Com_Server( Delta_Data::data_def_t const* delta_com_def,
												  Modbus_Net* modbus_net,
												  Delta_Com_Display* display,
												  Delta_Com_Server::cback_func_t callback,
												  Delta_Com_Server::cback_client_validate_func_t client_validate_callback,
												  Delta_Com_Server::cback_param_t parameter ) :
	m_delta_com( new Delta_Com_Server( delta_com_def,
									   reinterpret_cast<delta_com_identity_source_t const*>(
										   nullptr ),
									   display, callback, client_validate_callback,
									   parameter ) )
{
	m_delta_com->Set_Maximum_Message_Size( MODBUS_DCOM_BUFFER_MAX_SIZE );

	modbus_net->Attach_User_Function( DELTA_COM_FUNC_CODE, this, &Delta_Com_Process_Static );

	new Modbus_Negotiation( modbus_net );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Delta_Com_Server::Modbus_Delta_Com_Server( Delta_Com_Server* server,
												  Modbus_Net* modbus_net ) :
	m_delta_com( server )
{
	modbus_net->Attach_User_Function( DELTA_COM_FUNC_CODE, this, &Delta_Com_Process_Static );

	new Modbus_Negotiation( modbus_net );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Delta_Com_Server::~Modbus_Delta_Com_Server( void )
{
	BF_Lib::Exception::Destructor_Not_Supported();

	delete m_delta_com;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Delta_Com_Server::Delta_Com_Process( MB_FRAME_STRUCT* rx_struct,
													MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint16_t tx_data_len;
	bool message_processed;

	// Here we check to see if at least the frame was accepted.  No delta com messages really
	// will
	// be transmitted up to Modbus.  This is an all or nothing error.  We should return a
	// error code indicating major internal failure.
	message_processed = m_delta_com->Process_Delta_Com_Message( &rx_struct->data[MODBUS_OFFSET],
																( rx_struct->data_length -
																  MODBUS_OFFSET ),
																&tx_struct->data[MODBUS_OFFSET],
																&tx_data_len );

	tx_struct->data_length = tx_data_len + MODBUS_OFFSET;

	if ( message_processed == false )
	{
		return_status = MB_SLAVE_DEVICE_FAILURE_ERROR_CODE;
	}

	return ( return_status );
}

}	/* End namespace BF_Mbus for this module*/
