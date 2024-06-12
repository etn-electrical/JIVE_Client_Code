/**
 *****************************************************************************************
 *  @file Ble_Advertisement.h
 *
 *  @brief
 *
 *  @details
 *
 *  @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef BLE_ADVERTISEMENT_H
#define BLE_ADVERTISEMENT_H

#include "includes.h"

/**
 **************************************************************************************************
 *  @brief An abstract class for BLE advertisement packets.
 *  @details  Instances of this class will be passed into the BLE stack OR on advertisement request
 *  a class instance will be passed into the BLE stack.  The BLE stack only has to deal then
 *  with the retrieval of the packet details.  Not what kind of packet it is.
 *
 *  @n @b Usage:
 **************************************************************************************************
 */
class Ble_Advertisement
{
	public:

		/// The following constant does not include the preamble or CRC space.
		static const uint8_t MAX_BLE_ADV_PACKET_LENGTH = 31U;

		/**
		 *	@brief Constructor
		 *	@details
		 *	@return this
		 */
		Ble_Advertisement( void );

		/**
		 *  @brief Destructor
		 *	@details
		 *  @return None
		 */
		virtual ~Ble_Advertisement( void );

		/// ********  These access methods are utilized by the BLE stack. ********

		/**
		 *  @brief This function is used by the BLE stack to get the full advertisement frame.
		 *  @details This is called by the BLE stack to get the full packet.  Any modifications
		 *  that are necessary for the stack behavior or specific to this device should be done
		 *  in the BLE stack space.
		 *	@param packet
		 *  @return The total packet length returned.
		 */
		virtual uint8_t Get_Full_Packet( uint8_t* packet ) = 0;

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Ble_Advertisement( const Ble_Advertisement& object );
		Ble_Advertisement & operator =( const Ble_Advertisement& object );

};

#endif