/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module contains implementation for the Modbus Port Enable and disable.
 *
 *	@details
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_SERIAL_H
#define MODBUS_SERIAL_H

#include "Modbus_Defines.h"

/**
 ****************************************************************************************
 * @brief The Modbus_Serial class
 * @details it is being inherited by the Modbus_Net class.
 * All used for the Enable, disable, transmitting modbus frame and getting status of Modbus port.
 *
 ****************************************************************************************
 */
class Modbus_Serial
{
	public:

		typedef void* MB_RX_PROC_CBACK_HANDLE;
		typedef void (* MB_RX_PROC_CBACK)( MB_RX_PROC_CBACK_HANDLE handle,
										   MB_FRAME_STRUCT* rx_struct );

		/**
		 *  @brief Constructor
		 *  @details It does nothing here.
		 */
		Modbus_Serial( void ) {}

		/**
		 *  @brief Destructor
		 *  @details It does nothing here.
		 */
		virtual ~Modbus_Serial( void ) {}

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Modbus serial ports.
		 *  @details Their purpose is to Enable, disable, transmitting modbus frame and getting status of Modbus port
		 */
		/// "use_long_holdoff = true" means 1.75ms is used as the min msg holdoff.
		virtual void Enable_Port( MB_RX_PROC_CBACK rx_cback,
								  MB_RX_PROC_CBACK_HANDLE rx_cback_handle,
								  tx_mode_t modbus_tx_mode, parity_t parity_select = MODBUS_PARITY_EVEN,
								  uint32_t baud_rate = 19200, bool use_long_holdoff = true,
								  stop_bits_t stop_bits = MODBUS_1_STOP_BIT ) = nullptr;

		virtual void Disable_Port( void ) = nullptr;

		virtual bool Port_Enabled( void ) = nullptr;

		virtual bool Idle( void ) = nullptr;

		virtual void Send_Frame( MB_FRAME_STRUCT* tx_struct ) = nullptr;

	private:

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Modbus_Serial( const Modbus_Serial& object );
		Modbus_Serial & operator =( const Modbus_Serial& object );

};

#endif
