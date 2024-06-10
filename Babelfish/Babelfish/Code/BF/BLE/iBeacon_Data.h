/**
 *****************************************************************************************
 *  @file iBeacon_Data.h
 *
 *  @brief
 *
 *  @details
 *
 *  @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef IBEACON_DATA_H
#define IBEACON_DATA_H

#include "Ble_Advertisement.h"
#include "string.h"
#include "DCI_Owner.h"

class iBeacon_Data : public Ble_Advertisement
{
	public:

		/**
		 * @brief Constructor
		 * @details Create ibeacon packet
		 * @return this
		 */
		iBeacon_Data( DCI_Owner* ibeacon_major_number_owner,
					  DCI_Owner* ibeacon_minor_number_owner,
					  DCI_Owner* ibeacon_128_bit_uuid_owner,
					  DCI_Owner* tx_power_owner );

		/**
		 * @brief Destructor
		 *	@details This will get called when object of Eddystone_URL goes out of scope
		 * @return None
		 */
		~iBeacon_Data( void );

		/// ********  This access method is utilized by the BLE stack. >>>>>>>
		/**
		 * @brief Get the advertisement packet from the application to set it in the BLE stack.
		 * @param[out] packet: Buffer to carry the advertisement packet.
		 * @return Length of the advertisement packet.
		 */
		uint8_t Get_Full_Packet( uint8_t* packet );

		/// ********  These access methods are utilized by the Application. >>>>>>>
		/**
		 * @brief Sets the majornumber and minornumber to be sent into the advertisement packet.
		 * @param[in] majornumber: majornumber of the ibeacon
		 * @param[in] minornumber: minornumber of the ibeacon
		 * @return Result of the Set method (i.e) true or false
		 */
		bool Set_iBeacon_Major_Minor_Number( uint8_t major, uint8_t minor );

		/**
		 * @brief Sets the 128 bit uuid to be sent into the advertisement packet.
		 * @param uuid: 128 bit Proximity UUID of the ibeacon
		 * @return Result of the Set method (i.e) true or false
		 */
		bool Set_iBeacon_128_Bit_UUID( uint8_t* uuid );

		/**
		 * @brief Sets the signal power to be sent into the advertisement packet.
		 * @param power: Tx power
		 * @return Result of the Set method (i.e) true or false
		 */
		bool Set_iBeacon_Signal_Power( uint8_t* power );

	private:

		/**
		 * @brief The following constant help to fill length of ibeacon packet prefix
		 */
		static const uint8_t IBEACON_ADV_FLAG_LENGTH = 3U;
		static const uint8_t IBEACON_ADV_DATA_LENGTH = 1U;
		static const uint8_t IBEACON_ADV_DATA_TYPE_LENGTH = 1U;
		static const uint8_t IBEACON_COMPANY_ID_LENGTH = 2U;
		static const uint8_t IBEACON_TYPE_LENGTH = 1U;
		static const uint8_t IBEACON_RAMAINING_LENGTH = 1U;
		static const uint8_t IBEACON_128_BIT_UUID_LENGTH = 16U;
		static const uint8_t IBEACON_MAJOR_NUMBER_LENGTH = 2U;
		static const uint8_t IBEACON_MINOR_NUMBER_LENGTH = 2U;
		static const uint8_t IBEACON_TX_POWER_LENGTH = 1U;

		/**
		 * @brief The following constant help to fill the iBeacon data packet
		 */
		static const uint8_t IBEACON_ADV_FLAG_OFFSET = 0U;
		static const uint8_t IBEACON_ADV_DATA_OFFSET = 3U;
		static const uint8_t IBEACON_ADV_DATA_TYPE_OFFSET = 4U;
		static const uint8_t IBEACON_COMPANY_ID_OFFSET = 5U;
		static const uint8_t IBEACON_TYPE_OFFSET = 7U;
		static const uint8_t IBEACON_REMAINING_LENGTH_OFFSET = 8U;
		static const uint8_t IBEACON_128_BIT_BEACON_UUID_OFFSET = 9U;
		static const uint8_t IBEACON_MAJOR_NUMBER_OFFSET = 25U;
		static const uint8_t IBEACON_MINOR_NUMBER_OFFSET = 27U;
		static const uint8_t IBEACON_TX_POWER_OFFSET = 29U;

		/**
		 * @brief constants to hold to fill the ibeacon packet length
		 */
		static const uint8_t MAX_IBEACON_PACKET_DATA_LENGTH = 30U;

		/**
		 * @brief variable to hold the iBeacon data packet to be sent out
		 */
		uint8_t m_ibeacon_data_packet_to_send[MAX_IBEACON_PACKET_DATA_LENGTH] = { NULL };

		/**
		 * @brief constants to hold the sample iBeacon data packet and the prefix
		 */
		const uint8_t m_beacon_adv_flag[IBEACON_ADV_FLAG_LENGTH] = { 0x02, 0x01, 0x06 };
		const uint8_t m_beacon_adv_data_length = 0x1AU;
		const uint8_t m_beacon_adv_data_type = 0xFFU;
		const uint8_t m_beacon_company_id[IBEACON_COMPANY_ID_LENGTH] = { 0x4C, 0x00 };
		const uint8_t m_beacon_type = 0x02U;
		const uint8_t m_beacon_remaining_length = 0x15U;

		/**
		 * DCID owner for the Eddystone URL Advertisement packet
		 */
		DCI_Owner* m_ibeacon_major_number_owner;
		DCI_Owner* m_ibeacon_minor_number_owner;
		DCI_Owner* m_ibeacon_128_bit_uuid_owner;
		DCI_Owner* m_tx_power_owner;

		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		iBeacon_Data( const iBeacon_Data& object );
		iBeacon_Data & operator =( const iBeacon_Data& object );

};

#endif
