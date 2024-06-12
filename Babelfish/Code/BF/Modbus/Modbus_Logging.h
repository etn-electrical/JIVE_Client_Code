/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Module for interfacing Log_Mem and Modbus.
 *
 *	@details Implements logging APIs over Modbus protocol.  Will interpret the
 *	Modbus logging function code, retrieve or clear the data from the Logginng memory
 *	then send the data to client.
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_LOGGING_H_
#define MODBUS_LOGGING_H_

#include "Includes.h"
#include "Modbus_Defines.h"
#include "DCI_Defines.h"
#include "Modbus_Logging.h"
#include "Modbus_Net.h"
#include "Log_Mem.h"

namespace BF_Mbus
{
/**
 ****************************************************************************************
 * @brief This is a Modbus_Logging class
 * @details Modbus logging function code and command handler
 * @n @b Usage: It Provides the Logging interface between Modbus and Logging Memory.
 * This Modbus logging function code, retrieve or clear the data from the Logginng memory
 * and then send data response.
 ****************************************************************************************
 */
class Modbus_Logging
{
	public:
		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus_Logging interface
		 *  @param[in] modbus_net: Handle for modbus frame and callback API
		 *  @param[in] log_mem_handle_array: Array of pointer to multiple log memories.
		 *  @param[in] total_log_id: Total log mem interface declared.
		 *  @return None
		 */
		Modbus_Logging( Modbus_Net* modbus_net, BF_Misc::Log_Mem** log_mem_handle_array,
						uint8_t total_log_id );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_Logging goes out of
		 * scope
		 *  @return None
		 */
		~Modbus_Logging( void );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_Logging( const Modbus_Logging& object );
		Modbus_Logging & operator =( const Modbus_Logging& object );

		uint8_t Read_Log_Data( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Clear_Log_Mem( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Process_Logging_Message( MB_FRAME_STRUCT* rx_struct,
										 MB_FRAME_STRUCT* tx_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static uint8_t Process_Logging_Message_Static( modbus_message_callback_param_t param,
													   MB_FRAME_STRUCT* rx_struct,
													   MB_FRAME_STRUCT* tx_struct )
		{
			return ( ( ( Modbus_Logging* )param )->Process_Logging_Message( rx_struct,
																			tx_struct ) );
		}

		BF_Misc::Log_Mem** m_log_mem_handle_array;
		uint16_t m_tx_buff_size;
		uint8_t m_total_log_id;

		/*Logging Function Code*/
		static const uint8_t MB_LOGGING_FUNC_CODE = 110U;	// 0x6E
		/*Logging Command*/
		static const uint8_t RETRIEVE_LOG_COMMAND = 0U;
		static const uint8_t CLEAR_LOG_COMMAND = 1U;
};

}

#endif	/* MODBUS_LOGGING_H_ */
