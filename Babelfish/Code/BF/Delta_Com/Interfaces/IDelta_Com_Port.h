/*
 * IDelta_Com_Port.h
 *
 *  Created on: Aug 26, 2014
 *      Author: E9924418
 */

#ifndef IDELTA_COM_PORT_H_
#define IDELTA_COM_PORT_H_

#include "Includes.h"
#include "Delta_Com_Defines.h"


typedef enum delta_com_port_status_t
{
	DCOM_PORT_SUCCESS = 0,
	DCOM_PORT_FAILURE,
	DCOM_PORT_BUSY,
	DCOM_PORT_OUT_OF_RESOURCE,
	DCOM_PORT_INVALID_DCOM_ADDRESS
} delta_com_port_status;

// Port Device states
typedef enum delta_com_port_dev_state_t
{
	DCOM_PORT_DEV_NON_EXISTENT = 0,
	DCOM_PORT_DEV_READY_TO_ESTABLISH_LINK,
	DCOM_PORT_DEV_ESTABLISHING_LINK,
	DCOM_PORT_DEV_TRANSACTING,
	DCOM_PORT_DEV_IDLE,
	DCOM_PORT_DEV_TIMEOUT,
	DCOM_PORT_DEV_ERROR
} delta_com_port_dev_state;

class IDelta_Com_Client;

class IDelta_Com_Port
{
	public:
		virtual void Delta_Com_Client_Set( IDelta_Com_Client* dcom_client ) = 0;

		virtual delta_com_port_status Send_Data( delta_com_device_address address ) = 0;

		virtual delta_com_port_status Get_TX_Buffer( delta_com_device_address address, uint8_t** buffer,
													 uint16_t** tx_length ) = 0;

		virtual void Release_TX_Buffer( delta_com_device_address address ) = 0;

		virtual void Remove_Device( delta_com_device_address address ) = 0;

		virtual uint16_t Max_Payload_Size_Get() = 0;

		virtual uint16_t Max_Payload_Transmit_Time_Ms() = 0;

		virtual void Disable_Port() = 0;

};


#endif	/* IDELTA_COM_PORT_H_ */
