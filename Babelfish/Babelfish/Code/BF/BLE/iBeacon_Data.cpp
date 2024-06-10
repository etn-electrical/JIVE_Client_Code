/**
 *****************************************************************************************
 *   @file
 *   @details See header file for module overview.
 *   @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "iBeacon_Data.h"
#include "StdLib_MV.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iBeacon_Data::iBeacon_Data( DCI_Owner* ibeacon_major_number_owner,
							DCI_Owner* ibeacon_minor_number_owner,
							DCI_Owner* ibeacon_128_bit_uuid_owner,
							DCI_Owner* tx_power_owner ) :
	m_ibeacon_major_number_owner( ibeacon_major_number_owner ),
	m_ibeacon_minor_number_owner( ibeacon_minor_number_owner ),
	m_ibeacon_128_bit_uuid_owner( ibeacon_128_bit_uuid_owner ),
	m_tx_power_owner( tx_power_owner )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iBeacon_Data::~iBeacon_Data()
{
	delete m_ibeacon_major_number_owner;
	delete m_ibeacon_minor_number_owner;
	delete m_ibeacon_128_bit_uuid_owner;
	delete m_tx_power_owner;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t iBeacon_Data::Get_Full_Packet( uint8_t* packet )
{
	uint8_t temp_ibeacon_major_number[IBEACON_MAJOR_NUMBER_LENGTH];
	uint8_t temp_ibeacon_minor_number[IBEACON_MINOR_NUMBER_LENGTH];
	uint8_t temp_ibeacon_128_bit_uuid[IBEACON_128_BIT_UUID_LENGTH];
	uint8_t temp_ibeacon_signal_power;

	memset( temp_ibeacon_major_number, 0, sizeof( temp_ibeacon_major_number ) );
	memset( temp_ibeacon_minor_number, 0, sizeof( temp_ibeacon_minor_number ) );
	memset( temp_ibeacon_128_bit_uuid, 0, sizeof( temp_ibeacon_128_bit_uuid ) );

	/// The following methods checkout the ibeacon data from the DCID's.
	m_ibeacon_major_number_owner->Check_Out( temp_ibeacon_major_number );
	m_ibeacon_minor_number_owner->Check_Out( temp_ibeacon_minor_number );
	m_ibeacon_128_bit_uuid_owner->Check_Out( temp_ibeacon_128_bit_uuid );
	m_tx_power_owner->Check_Out( temp_ibeacon_signal_power );

	/// The following lines of code shall frame the complete 31Bytes of advertisement packet
	BF_Lib::Copy_String( &m_ibeacon_data_packet_to_send[IBEACON_ADV_FLAG_OFFSET],
						 m_beacon_adv_flag, IBEACON_ADV_FLAG_LENGTH );

	m_ibeacon_data_packet_to_send[IBEACON_ADV_DATA_OFFSET] = m_beacon_adv_data_length;

	m_ibeacon_data_packet_to_send[IBEACON_ADV_DATA_TYPE_OFFSET] = m_beacon_adv_data_type;

	BF_Lib::Copy_String( &m_ibeacon_data_packet_to_send[IBEACON_COMPANY_ID_OFFSET],
						 m_beacon_company_id, IBEACON_COMPANY_ID_LENGTH );

	m_ibeacon_data_packet_to_send[IBEACON_TYPE_OFFSET] = m_beacon_type;

	m_ibeacon_data_packet_to_send[IBEACON_REMAINING_LENGTH_OFFSET] = m_beacon_remaining_length;

	BF_Lib::Copy_String( &m_ibeacon_data_packet_to_send[IBEACON_128_BIT_BEACON_UUID_OFFSET],
						 temp_ibeacon_128_bit_uuid, IBEACON_128_BIT_UUID_LENGTH );

	BF_Lib::Copy_String( &m_ibeacon_data_packet_to_send[IBEACON_MAJOR_NUMBER_OFFSET],
						 temp_ibeacon_major_number, IBEACON_MAJOR_NUMBER_LENGTH );

	BF_Lib::Copy_String( &m_ibeacon_data_packet_to_send[IBEACON_MINOR_NUMBER_OFFSET],
						 temp_ibeacon_minor_number, IBEACON_MINOR_NUMBER_LENGTH );

	m_ibeacon_data_packet_to_send[IBEACON_TX_POWER_OFFSET] = temp_ibeacon_signal_power;

	BF_Lib::Copy_String( packet, m_ibeacon_data_packet_to_send, MAX_IBEACON_PACKET_DATA_LENGTH );

	return ( MAX_IBEACON_PACKET_DATA_LENGTH );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool iBeacon_Data::Set_iBeacon_Major_Minor_Number( uint8_t major, uint8_t minor )
{
	m_ibeacon_major_number_owner->Check_In( major );
	m_ibeacon_minor_number_owner->Check_In( minor );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool iBeacon_Data::Set_iBeacon_128_Bit_UUID( uint8_t* uuid )
{
	m_ibeacon_128_bit_uuid_owner->Check_In( uuid );
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool iBeacon_Data::Set_iBeacon_Signal_Power( uint8_t* power )
{
	m_tx_power_owner->Check_In( power );
	return ( true );
}