/**
 *****************************************************************************************
 *   @file
 *   @details See header file for module overview.
 *   @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Eddystone_URL.h"
#include "StdLib_MV.h"
#include "String.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Eddystone_URL::Eddystone_URL( DCI_Owner* url_owner, DCI_Owner* tx_power_owner ) :
	Ble_Advertisement(),
	m_url_owner( url_owner ),
	m_tx_power_owner( tx_power_owner )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Eddystone_URL::~Eddystone_URL()
{
	delete m_url_owner;
	delete m_tx_power_owner;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Eddystone_URL::Get_Full_Packet( uint8_t* packet )
{
	uint8_t temp_URL[MAX_EDDYSTONE_URL_DATA_LENGTH];
	int8_t temp_power;
	uint8_t total_packet_length;
	uint8_t url_length;
	uint8_t loop = 0U;

	memset( temp_URL, 0U, sizeof( temp_URL ) );

	/// The following methods checkout the Eddystone URL and Tx power from the DCID's.
	m_tx_power_owner->Check_Out( temp_power );
	m_url_owner->Check_Out( temp_URL );

	while ( ( loop < MAX_EDDYSTONE_URL_DATA_LENGTH ) && ( temp_URL[loop] != '!' ) )
	{
		loop++;
	}

	url_length = loop;

	/// The following lines of code shall frame the complete 31Bytes of advertisement packet
	BF_Lib::Copy_String( &m_eddystone_URL_packet_to_send[EDDYSTONE_16_BIT_UUID_OFFSET],
						 m_16bit_Service_UUID_AD_element, MAX_EDDYSTONE_16_BIT_UUID_AD_ELEMENT_LENGTH );

	m_eddystone_URL_packet_to_send[EDDYSTONE_URL_AD_ELEMENT_OFFSET] =
		EDDYSTONE_URL_FIXED_LENGTH + url_length;

	BF_Lib::Copy_String( &m_eddystone_URL_packet_to_send[EDDYSTONE_URL_HEADER_OFFSET],
						 m_eddystone_url_ad_element_header, EDDYSTONE_URL_HEADER_LENGTH );

	m_eddystone_URL_packet_to_send[TX_POWER_OFFSET] = temp_power;

	BF_Lib::Copy_String( &m_eddystone_URL_packet_to_send[EDDYSTONE_URL_OFFSET], temp_URL,
						 url_length );

	total_packet_length = EDDYSTONE_16_BIT_UUID_FIXED_LENGTH + EDDYSTONE_URL_AD_ELEMENT_LENGTH +
		EDDYSTONE_URL_FIXED_LENGTH + url_length;

	BF_Lib::Copy_String( packet, m_eddystone_URL_packet_to_send, total_packet_length );

	return ( total_packet_length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Eddystone_URL::Set_URL( uint8_t* url, uint16_t offset, uint8_t length )
{
	m_url_owner->Check_In_Offset( url, offset, length );
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Eddystone_URL::Set_TX_Power( int8_t power )
{
	m_tx_power_owner->Check_In( power );
	return ( true );
}
