/**
 **************************************************************************************************
 *  @file Eddystone_URL.h
 *
 *  @brief An eddystone implementation
 *
 *  @details
 *
 *  @n @b Usage:
 *  @code
    uint8_t advdata[] =
    {
        0x03,  // Length of Service List - Fixed.
        0x03,  // Param: Service List - Fixed
        0xAA, 0xFE,  // Eddystone ID - Fixed
        0x0C,  // Length of Service Data - Configured based on URL.
        0x16,  // Service Data
        0xAA, 0xFE, // Eddystone ID
        0x10,  // Frame type: URL
        0xF8, // Power
        0x03, // https://
        'e',
        'a',
        't',
        'o',
        'n',
        0x07, // .com
    };
 *  @endcode
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *  @copyright 2018 Eaton Corporation. All Rights Reserved.
 **************************************************************************************************
 */

#ifndef EDDYSTONE_URL_H
#define EDDYSTONE_URL_H

#include "Ble_Advertisement.h"
#include "DCI_Owner.h"

class Eddystone_URL : public Ble_Advertisement
{
	public:

		/**
		 * @brief Constructor
		 * @details Create Eddystone URL packet
		 * @return this
		 */
		Eddystone_URL( DCI_Owner* url_owner, DCI_Owner* tx_power_owner );

		/**
		 * @brief Destructor
		 * @details This will get called when object of Eddystone_URL goes out of scope
		 * @return None
		 */
		~Eddystone_URL( void );

		/// ********  These access method is utilized by the BLE stack. >>>>>>>
		/**
		 * @brief Get the advertisement packet from the application to set it in the BLE stack.
		 * @param[out] packet: Buffer to carry the advertisement packet.
		 * @return Length of the advertisement packet.
		 */
		uint8_t Get_Full_Packet( uint8_t* packet );

		/// ********  These access method are utilized by the Application. >>>>>>>
		/**
		 * @brief Sets the URL to be sent into the advertisement packet.
		 * @param[in] url: Buffer that carries the url
		 * @param[in] length: Length of the url data
		 * @return Result of the Set method (i.e) true or false
		 */
		bool Set_URL( uint8_t* url, uint16_t offset, uint8_t length );

		/**
		 * @brief Sets the URL to be sent into the advertisement packet.
		 * @param[in] power: In the range from -100 to +20 in single increments.
		 * @return Result of the Set method (i.e) true or false
		 */
		bool Set_TX_Power( int8_t power );

	private:

		/**
		 * @brief The following constant include the Eddystone service class UUID and
		 * Eddystone URL header.
		 */
		static const uint8_t MAX_EDDYSTONE_16_BIT_UUID_AD_ELEMENT_LENGTH = 6U;
		static const uint8_t EDDYSTONE_URL_HEADER_LENGTH = 4U;

		/**
		 * @brief The following constant helps to fill the Eddystone advertisement data
		 */
		static const uint8_t EDDYSTONE_16_BIT_UUID_OFFSET = 0U;
		static const uint8_t EDDYSTONE_URL_AD_ELEMENT_OFFSET = 6U;
		static const uint8_t EDDYSTONE_URL_HEADER_OFFSET = 7U;
		static const uint8_t TX_POWER_OFFSET = 11U;
		static const uint8_t EDDYSTONE_URL_OFFSET = 12U;

		/**
		 * @brief The following constant helps to fill the Eddystone advertisement
		 * packet data length.
		 */
		static const uint8_t EDDYSTONE_16_BIT_UUID_FIXED_LENGTH = 6U;
		static const uint8_t EDDYSTONE_URL_AD_ELEMENT_LENGTH = 1U;
		static const uint8_t EDDYSTONE_URL_FIXED_LENGTH = 5U;
		static const uint8_t MAX_EDDYSTONE_URL_DATA_LENGTH = 18U;

		/**
		 * @brief constants to hold the Eddystone 16Bit UUID AdElement amd
		 * Eaton 16 Bit UUID AD Elemment and Eddystone URL header fixed bytes
		 */
		const uint8_t m_16bit_Service_UUID_AD_element[MAX_EDDYSTONE_16_BIT_UUID_AD_ELEMENT_LENGTH] =
		{ 0x05, 0x03, 0xAA, 0xFE, 0x03, 0x50 };
		const uint8_t m_eddystone_url_ad_element_header[EDDYSTONE_URL_HEADER_LENGTH] =
		{ 0x16, 0xAA, 0xFE, 0x10 };
		/**
		 * @brief variable to hold the Eddystone URL Advertisement packet to be sent out
		 */
		uint8_t m_eddystone_URL_packet_to_send[MAX_BLE_ADV_PACKET_LENGTH] = { 0U };

		/**
		 * @brief DCID owner for the Eddystone URL Advertisement packet and the Tx power
		 */
		DCI_Owner* m_url_owner;
		DCI_Owner* m_tx_power_owner;

		/**
		 * @brief Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Eddystone_URL( const Eddystone_URL& object );
		Eddystone_URL & operator =( const Eddystone_URL& object );

};

#endif
