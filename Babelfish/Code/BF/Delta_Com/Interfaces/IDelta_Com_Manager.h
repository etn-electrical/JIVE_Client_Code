/*
 * IDelta_Com_Manager.h
 *
 *  Created on: Sep 12, 2014
 *      Author: E9924418
 */

#ifndef IDELTA_COM_MANAGER_H_
#define IDELTA_COM_MANAGER_H_

#include "Delta_Com_Defines.h"
#include "Delta_Com_Identity.h"
#include "IDelta_Com_Client.h"

typedef enum delta_com_manager_status_t
{
	DCOM_MANAGER_SUCCESS = 0,
	DCOM_MANAGER_FAILURE,
	DCOM_MANAGER_DEVICE_INCOMPATIBLE,
	DCOM_MANAGER_OUT_OF_RESOURCE
} delta_com_manager_status;

class Delta_Data;

class IDelta_Com_Manager
{
	public:
		virtual void Delta_Com_Client_Dev_State_Change( uint8_t client_id,
														delta_com_device_address dcom_dev_address,
														delta_com_client_dev_state dcom_port_dev_state ) = 0;

		virtual Delta_Data* New_Delta_Com_Device_Add( uint8_t client_id,
													  delta_com_device_address dcom_dev_address,
													  delta_com_identity_t* id_info ) = 0;

};


#endif	/* IDELTA_COM_MANAGER_H_ */
