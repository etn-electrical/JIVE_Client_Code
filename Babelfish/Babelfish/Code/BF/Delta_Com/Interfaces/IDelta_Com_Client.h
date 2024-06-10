/*
 * IDelta_Com_Client.h
 *
 *  Created on: Aug 26, 2014
 *      Author: E9924418
 */

#ifndef IDELTA_COM_CLIENT_H_
#define IDELTA_COM_CLIENT_H_

#include "Delta_Com_Defines.h"
#include "IDelta_Com_Port.h"

typedef enum delta_com_client_status_t
{
	DCOM_CLIENT_SUCCESS = 0,
	DCOM_CLIENT_FAILURE,
	DCOM_CLIENT_OUT_OF_RESOURCE
} delta_com_client_status;

// Port Device states
typedef enum delta_com_client_dev_state_t
{
	DCOM_CLIENT_DEV_NON_EXISTENT = 0,
	DCOM_CLIENT_DEV_REMOVED_OR_LOST,
	DCOM_CLIENT_DEV_NEW_DEVICE_DISCOVERED,
	DCOM_CLIENT_DEV_COMM_FAULT,
	DCOM_CLIENT_DEV_PERFORMING_IDENTITY_EXCHANGE,
	DCOM_CLIENT_DEV_IDENTITY_MISMATCH,
	DCOM_CLIENT_DEV_STARTING_MANAGE_SUBSCRIPTIONS,
	DCOM_CLIENT_DEV_STARTING_PARAMETER_SYNC,
	DCOM_CLIENT_DEV_CONNECTION_COMPLETE_DEVICE_ACTIVE,
	DCOM_CLIENT_DEV_MISSED_FRAME_RETRANSMITTING
} delta_com_client_dev_state;

class IDelta_Com_Client
{
	public:
		virtual delta_com_client_status Delta_Com_Port_Dev_State_Change( delta_com_device_address dcom_dev_address,
																		 delta_com_port_dev_state dcom_port_dev_state )
		= 0;

		virtual void Delta_Com_Response( uint8_t* data, uint16_t length,
										 delta_com_device_address dcom_dev_address ) = 0;

};

#endif	/* IDELTA_COM_CLIENT_H_ */
