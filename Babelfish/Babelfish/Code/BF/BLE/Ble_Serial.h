/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module contains implementation for the Ble Port Enable and disable.
 *
 *	@details
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BLE_SERIAL_H
#define BLE_SERIAL_H

#include "Ble_Defines.h"

/**
 ****************************************************************************************
 * @brief The Ble_Serial class
 * @details it is being inherited by the Ble_Net class.
 * All used for the Enable, disable, transmitting Ble frame and getting status of Ble port.
 *
 ****************************************************************************************
 */
class Ble_Serial
{
	public:

		typedef struct
		{
			uint8_t* data;
			uint16_t data_length;
		} ble_frame_struct_t;


		/**
		 *  @brief Constructor
		 *  @details It does nothing here.
		 */
		Ble_Serial( void ) {}

		/**
		 *  @brief Destructor
		 *  @details It does nothing here.
		 */
		virtual ~Ble_Serial( void ) {}

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Ble serial ports.
		 *  @details Their purpose is to Enable, disable, transmitting Ble frame and
		    getting status of Ble port
		 */
		virtual void Enable_Port( ble_parity_t parity_select = BLE_PARITY_NONE,
								  uint32_t baud_rate = 115200U,
								  ble_stop_bits_t stop_bits = BLE_1_STOP_BIT ) = 0;

		virtual void Disable_Port( void ) = 0;

		virtual bool Port_Enabled( void ) = 0;

		virtual void Send_Frame( ble_frame_struct_t* tx_struct ) = 0;

	private:

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Ble_Serial( const Ble_Serial& object );
		Ble_Serial & operator =( const Ble_Serial& object );

};

#endif
