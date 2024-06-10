/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Module for interfacing delta com and Modbus.
 *
 *	@details Provides the Modbus Server interface for Delta Com.  Will interpret the
 *	Modbus delta com function code, send the message to delta com for processing and
 *	then send an appropriate response.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_DELTA_COM_SERVER_H_
#define MODBUS_DELTA_COM_SERVER_H_

#include "Includes.h"
#include "Modbus_Defines.h"
#include "DCI_Defines.h"
#include "Delta_Com_Server.h"
#include "Modbus_Net.h"
#include "Delta_Data.h"

namespace BF_Mbus
{
/**
 ****************************************************************************************
 * @brief This is a Modbus_Delta_Com_Server class
 * @details Modbus Delta Com user function code handler
 * @n @b Usage: It Provides the Modbus Server interface for Delta Com.
 * Will interpret the Modbus delta com function code, send the message to delta com for
 * processing and
 *	then send an appropriate response.It provides an interface to Create the Patron,
 *	provide the data access to DC database.
 ****************************************************************************************
 */
class Modbus_Delta_Com_Server
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus delta Communication Server
		 *  @param[in] source_id of communication protocol
		 *  @param[in] delta_com_def structure
		 *  @param[in] modbus_net is pointer to Modbus Net
		 *  @n @b Usage: Creates a Modbus Host port
		 *  @param[in] display indicate to modbus delta_com communication
		 *  @n @b Usage: confirmation to delta_com communication
		 *  @param[in] callback: Function pointer to Callback function
		 *  @n @b Usage:Make sure callback function is defined
		 *  @param[in] parameter: parameters for callback function
		 *  @n @b Usage: By default its initialized with nullptr
		 *  @return None
		 */
		Modbus_Delta_Com_Server( Delta_Data::data_def_t const* delta_com_def,
								 Modbus_Net* modbus_net,
								 Delta_Com_Display*
								 display = reinterpret_cast<Delta_Com_Display*>( nullptr ),
								 Delta_Com_Server::cback_func_t callback =
								 reinterpret_cast<Delta_Com_Server::cback_func_t>( nullptr ),
								 Delta_Com_Server::cback_client_validate_func_t client_validate_callback =
								 reinterpret_cast<Delta_Com_Server::
												  cback_client_validate_func_t>( nullptr ),
								 Delta_Com_Server::cback_param_t parameter =
								 reinterpret_cast<Delta_Com_Server::cback_param_t>( nullptr ) );

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus delta Communication Server
		 *  @param[in] server: It initialize delta com server
		 *  @n @b Usage: make sure server is initialized to communicate
		 *  @param[in] modbus_net is pointer to Modbus Net
		 *  @n @b Usage: Creates a Modbus Host port
		 *  @return None
		 */
		Modbus_Delta_Com_Server( Delta_Com_Server* server, Modbus_Net* modbus_net );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_Delta_Com_Server goes out of
		 * scope
		 *  @return None
		 */
		~Modbus_Delta_Com_Server( void );

		// The following should be used to populate the identity struct which contains the max
		// buffer size.
		static const uint16_t MODBUS_DCOM_BUFFER_MAX_SIZE = MODBUS_MAX_RTU_MSG_DATA_LEN;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_Delta_Com_Server( const Modbus_Delta_Com_Server& object );
		Modbus_Delta_Com_Server & operator =( const Modbus_Delta_Com_Server& object );

		Delta_Com_Server* m_delta_com;

		uint8_t Delta_Com_Process( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static uint8_t Delta_Com_Process_Static( modbus_message_callback_param_t param,
												 MB_FRAME_STRUCT* rx_struct,
												 MB_FRAME_STRUCT* tx_struct )
		{
			return ( ( ( Modbus_Delta_Com_Server* )param )->Delta_Com_Process( rx_struct,
																			   tx_struct ) );
		}

		static const uint8_t MODBUS_OFFSET = 2U;
		static const uint8_t DELTA_COM_FUNC_CODE = 0x46U;
		static const uint8_t DELTA_COM_OVERHEAD = 0x06U;
};

}

#endif	/* MODBUS_DELTA_COM_H_ */
