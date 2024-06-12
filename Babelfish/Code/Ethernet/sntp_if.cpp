/**
 **********************************************************************************************
 *	@file            sntp_if.cpp C++ Implementation File for Ethernet common functions.
 *
 *	@brief           The file shall include the definitions of all the functions required for
 *                   Ethernet initilization.
 *	@details
 *	@copyright       2014 Eaton Corporation. All Rights Reserved.
 *	@remark			 Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "sntp_if.h"
#ifdef ESP32_SETUP
#include "esp_sntp.h"
#else
#include "lwip/apps/sntp.h"
#endif
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#ifdef SNTP_DOMAIN_NAME_SUPPORT
#include "String_Sanitizer.hpp"
#include "String_Sanitize_Whitelists.h"
#endif
#include "Format_Handler.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#ifdef SNTP_DOMAIN_NAME_SUPPORT
const BF_Lib::String_Sanitizer sntp_dns_whitelist( sntp_dns_sanitize_lookup );
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
SNTP_If::SNTP_If( cback_t* cback, cback_param_t cback_param ) :
	///< Initialised with true to check dcid DCI_SNTP_SERVICE_ENABLE with every boot cycle.
	m_sntp_update_required( true ),
	m_owner_sntp_service_enable( new DCI_Owner( DCI_SNTP_SERVICE_ENABLE_DCID ) ),
	m_owner_sntp_server_1( new DCI_Owner( DCI_SNTP_SERVER_1_DCID ) ),
	m_owner_sntp_server_2( new DCI_Owner( DCI_SNTP_SERVER_2_DCID ) ),
	m_owner_sntp_server_3( new DCI_Owner( DCI_SNTP_SERVER_3_DCID ) ),
	m_owner_sntp_update_time( new DCI_Owner( DCI_SNTP_UPDATE_TIME_DCID ) ),
	m_owner_sntp_retry_time( new DCI_Owner( DCI_SNTP_RETRY_TIME_DCID ) ),
	m_owner_sntp_active_server_status( new DCI_Owner( DCI_SNTP_ACTIVE_SERVER_DCID ) ),
	m_owner_sntp_time_offset( new DCI_Owner( DCI_SNTP_TIME_OFFSET_DEBUG_DCID ) ),
	m_rtc_update_cb( cback ),
	m_rtc_update_cb_param( cback_param )
{
	m_owner_sntp_service_enable->Attach_Callback( Sntp_Service_Callback_Static,
												  reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
												  ( DCI_ACCESS_POST_SET_RAM_CMD_MSK ) );

	m_owner_sntp_server_1->Attach_Callback( Sntp_Service_Callback_Static,
											reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											( DCI_ACCESS_SET_RAM_CMD_MSK ) );

	m_owner_sntp_server_2->Attach_Callback( Sntp_Service_Callback_Static,
											reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											( DCI_ACCESS_SET_RAM_CMD_MSK ) );

	m_owner_sntp_server_3->Attach_Callback( Sntp_Service_Callback_Static,
											reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											( DCI_ACCESS_SET_RAM_CMD_MSK ) );

	m_owner_sntp_update_time->Attach_Callback( Sntp_Service_Callback_Static,
											   reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											   ( DCI_ACCESS_POST_SET_RAM_CMD_MSK ) );

	m_owner_sntp_retry_time->Attach_Callback( Sntp_Service_Callback_Static,
											  reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											  ( DCI_ACCESS_POST_SET_RAM_CMD_MSK ) );

	///< Set Server - Domain Name or IP address to sntp_server list
	if ( m_sntp_server_1 == nullptr )
	{
		m_sntp_server_1 = new uint8_t[m_owner_sntp_server_1->Get_Length()];
	}

	///< Set Server - Domain Name or IP address to sntp_server list
	if ( m_sntp_server_2 == nullptr )
	{
		m_sntp_server_2 = new uint8_t[m_owner_sntp_server_2->Get_Length()];
	}

	///< Set Server - Domain Name or IP address to sntp_server list
	if ( m_sntp_server_3 == nullptr )
	{
		m_sntp_server_3 = new uint8_t[m_owner_sntp_server_3->Get_Length()];
	}

	///< Set up sntp server list by validating the IP Address or domain name from the DCID.
	Set_Sntp_Server_List( m_sntp_server_1, m_owner_sntp_server_1, SNTP_SERVER_1_INDEX, nullptr );
	Set_Sntp_Server_List( m_sntp_server_2, m_owner_sntp_server_2, SNTP_SERVER_2_INDEX, nullptr );
	Set_Sntp_Server_List( m_sntp_server_3, m_owner_sntp_server_3, SNTP_SERVER_3_INDEX, nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD SNTP_If::Sntp_Service_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	///< check if command is valid
	if ( access_struct->command == DCI_ACCESS_POST_SET_RAM_CMD )
	{
		switch ( access_struct->data_id )
		{
			case DCI_SNTP_SERVICE_ENABLE_DCID:
			case DCI_SNTP_UPDATE_TIME_DCID:
			case DCI_SNTP_RETRY_TIME_DCID:
				m_sntp_update_required = true;
#ifdef SNTP_OFFSET_DEBUG
				// setting offset index to zero to collect new samples
				m_sntp_time_offset_index = 0;
#endif
				break;

			default:
				return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
				break;
		}
	}
	else if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD )
	{
		uint8_t sntp_operation = 0;
		uint8_t sntp_active_server_status = 0;
		switch ( access_struct->data_id )
		{
			case DCI_SNTP_SERVER_1_DCID:
#ifdef SNTP_DOMAIN_NAME_SUPPORT
				///< Sanitize the user entry using the lookup as per the host name standards
				return_status = Sntp_Dns_Check_Whitelist_Sanitize( access_struct );
				if ( return_status == DCI_CBACK_RET_PROCESS_NORMALLY )
				{
#endif
				///< Set up sntp server 1 by validating the IP Address or domain name from the User Entry.
				return_status = Set_Sntp_Server_List( m_sntp_server_1, m_owner_sntp_server_1, SNTP_SERVER_1_INDEX,
													  access_struct );

				if ( return_status == DCI_CBACK_RET_PROCESS_NORMALLY )
				{
					///< Trigger sntp service, Based on operation command and server connection status
					m_owner_sntp_service_enable->Check_Out( sntp_operation );
					m_owner_sntp_active_server_status->Check_Out(
						sntp_active_server_status );

					if ( ( SERVER_1_CONNECTED == sntp_active_server_status ) ||
						 ( ( SNTP_SERVICE_DISABLE != sntp_operation ) &&
						   ( DISCONNECTED == sntp_active_server_status ) ) )
					{
						///< Trigger sntp service when server_1 ip changed or when device was SNTP
						///< service enabled but disconnected from time server.
						m_sntp_update_required = true;

					}
				}
#ifdef SNTP_DOMAIN_NAME_SUPPORT
		}
#endif
				break;

			case DCI_SNTP_SERVER_2_DCID:
#ifdef SNTP_DOMAIN_NAME_SUPPORT
				///< Sanitize the user entry using the lookup as per the host name standards
				return_status = Sntp_Dns_Check_Whitelist_Sanitize( access_struct );
				if ( return_status == DCI_CBACK_RET_PROCESS_NORMALLY )
				{
#endif
				///< Set up sntp server 2 by validating the IP Address or domain name from the User Entry.
				return_status = Set_Sntp_Server_List( m_sntp_server_2, m_owner_sntp_server_2, SNTP_SERVER_2_INDEX,
													  access_struct );

				if ( return_status == DCI_CBACK_RET_PROCESS_NORMALLY )
				{
					///< Trigger sntp service, Based on operation command and server connection status
					m_owner_sntp_service_enable->Check_Out( sntp_operation );
					m_owner_sntp_active_server_status->Check_Out(
						sntp_active_server_status );

					if ( ( SERVER_2_CONNECTED == sntp_active_server_status ) ||
						 ( ( SNTP_SERVICE_DISABLE != sntp_operation ) &&
						   ( DISCONNECTED == sntp_active_server_status ) ) )
					{
						///< Trigger sntp service when server_2 ip changed or when device was SNTP
						///< service enabled but disconnected from time server.
						m_sntp_update_required = true;

					}
				}
#ifdef SNTP_DOMAIN_NAME_SUPPORT
		}
#endif
				break;

			case DCI_SNTP_SERVER_3_DCID:
#ifdef SNTP_DOMAIN_NAME_SUPPORT
				///< Sanitize the user entry using the lookup as per the host name standards
				return_status = Sntp_Dns_Check_Whitelist_Sanitize( access_struct );
				if ( return_status == DCI_CBACK_RET_PROCESS_NORMALLY )
				{
#endif
				///< Set up sntp server 3 by validating the IP Address or domain name from the User Entry.
				return_status = Set_Sntp_Server_List( m_sntp_server_3, m_owner_sntp_server_3, SNTP_SERVER_3_INDEX,
													  access_struct );

				if ( return_status == DCI_CBACK_RET_PROCESS_NORMALLY )
				{
					///< Trigger sntp service, Based on operation command and server connection status
					m_owner_sntp_service_enable->Check_Out( sntp_operation );
					m_owner_sntp_active_server_status->Check_Out(
						sntp_active_server_status );

					if ( ( SERVER_3_CONNECTED == sntp_active_server_status ) ||
						 ( ( SNTP_SERVICE_DISABLE != sntp_operation ) &&
						   ( DISCONNECTED == sntp_active_server_status ) ) )
					{
						///< Trigger sntp service when server_3 ip changed or when device was SNTP
						///< service enabled but disconnected from time server.
						m_sntp_update_required = true;
					}
				}
#ifdef SNTP_DOMAIN_NAME_SUPPORT
		}
#endif
				break;

			default:
				return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
				break;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_If::Update( void )
{
	uint8_t sntp_operation = SNTP_SERVICE_DISABLE;

	sys_timeout( SNTP_POLLING_TIME, Update_Static, reinterpret_cast<void*>( this ) );

	if ( m_sntp_update_required == true )
	{
		/// Read DCID_sntp_enable
		m_owner_sntp_service_enable->Check_Out( sntp_operation );
		if ( SNTP_SERVICE_ENABLE_SERVER_ENTRY_VIA_USER == sntp_operation )
		{
			/// Check network interface link is up
			if ( netif_is_up( netif_list ) )
			{
				/// Refresh sntp service
#ifdef ESP32_SETUP
				sntp_stop();
				sntp_init();
#else
				sntp_stop();
				sntp_enable();
#endif
				/// Clear Flag
				m_sntp_update_required = false;
			}
		}
		else
		{
			/// User wants SNTP = OFF
			sntp_stop();
			/// Clear Flag
			m_sntp_update_required = false;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_If::Set_Active_Server_Status( uint8_t connection_status,
										uint8_t server_index )
{
	uint8_t current_status = NOT_CONNECTED;

	/// Update Active Server status
	if ( CONNECTED == connection_status )
	{
		/// When server_index = 0 , we are actually connected to sntp_server_1
		current_status = server_index + 1;
	}

	///< Update DCI_SNTP_ACTIVE_SERVER_DCID
	m_owner_sntp_active_server_status->Check_In( current_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_If::Set_Epoch_Time( uint32_t& sec, uint32_t& usec )
{
	// Set Epoch time by application space callback function
	m_rtc_update_cb( m_rtc_update_cb_param, NEW_TIME_RECEIVED, &sec, &usec );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_If::Get_Epoch_Time( uint32_t& sec, uint32_t& usec )
{
	// Get Epoch time by application space callback function
	m_rtc_update_cb( m_rtc_update_cb_param, RETRIEVE_TIME, &sec, &usec );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t SNTP_If::Get_Sntp_Update_Time( void )
{
	uint32_t sntp_update_time = 0U;

	m_owner_sntp_update_time->Check_Out( sntp_update_time );
	return ( sntp_update_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t SNTP_If::Get_Sntp_Retry_Time( void )
{
	uint16_t sntp_retry_time = 0U;

	m_owner_sntp_retry_time->Check_Out( sntp_retry_time );
	return ( sntp_retry_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_If::Calculate_Offset_Time( uint32_t* t1, uint32_t* t2, uint32_t* t3, uint32_t* t4,
									 uint32_t* offset )
{
	uint32_t time_offset[2] = { 0 };
	int64_t time_1 = 0;
	int64_t time_2 = 0;
	int64_t time_3 = 0;
	int64_t time_4 = 0;
	int64_t result = 0;

	int64_t temp_1 = 0;
	int64_t temp_2 = 0;

	int64_t temp_const_1 = static_cast<int64_t>( SEC_TO_MICRO_SEC_SCALE );
	int64_t temp_const_2 = static_cast<int64_t>( SNTP_PICO_TO_MICRO_SEC_SCALE );

	// Offset time 1
	temp_1 = static_cast<int64_t>( ntohl( t1[0] ) );
	temp_1 = temp_1 * temp_const_1;
	temp_2 = static_cast<int64_t>( ntohl( t1[1] ) );
	time_1 = temp_1 + temp_2;

	// Offset time 2
	temp_1 = static_cast<int64_t>( ntohl( t2[0] ) );
	temp_1 = temp_1 * temp_const_1;
	temp_2 = static_cast<int64_t>( ntohl( t2[1] ) );
	temp_2 = temp_2 / temp_const_2;
	time_2 = temp_1 + temp_2;

	// Offset time 3
	temp_1 = static_cast<int64_t>( ntohl( t3[0] ) );
	temp_1 = temp_1 * temp_const_1;
	temp_2 = static_cast<int64_t>( ntohl( t3[1] ) );
	temp_2 = temp_2 / temp_const_2;
	time_3 = temp_1 + temp_2;

	// Offset time 4
	temp_1 = static_cast<int64_t>( ntohl( t4[0] ) );
	temp_1 = temp_1 * temp_const_1;
	temp_2 = static_cast<int64_t>( ntohl( t4[1] ) );
	time_4 = temp_1 + temp_2;

	result = time_4 + ( ( ( time_2 - time_1 ) + ( time_3 - time_4 ) ) / 2 );

#ifdef SNTP_OFFSET_DEBUG
	int64_t max_offset_in_samples = 0xFFFF;

	// Checkin the default value of 0xFFFF to avoid having old max offset
	// while start collecting the new offset samples
	if ( m_sntp_time_offset_index == 0U )
	{
		m_owner_sntp_time_offset->Check_In( &max_offset_in_samples );
	}

	// Calculating the absolute value of NTP offset and filling the samples in buffer
	m_sntp_time_offset_samples[m_sntp_time_offset_index++] = abs( ( ( time_2 - time_1 ) + ( time_3 - time_4 ) ) / 2 );

	if ( m_sntp_time_offset_index == MAX_SNTP_OFFSET_SAMPLES )
	{
		// skip the first sample since the offset will be huge during the first time sync
		for ( uint8_t i = VALID_SNTP_OFFSET_INDEX + 1; i < MAX_SNTP_OFFSET_SAMPLES; ++i )
		{
			if ( m_sntp_time_offset_samples[VALID_SNTP_OFFSET_INDEX] < m_sntp_time_offset_samples[i] )
			{
				m_sntp_time_offset_samples[VALID_SNTP_OFFSET_INDEX] = m_sntp_time_offset_samples[i];
			}
		}

		// storing the max offset on the positive threshold
		max_offset_in_samples = m_sntp_time_offset_samples[VALID_SNTP_OFFSET_INDEX];

		// checkin to the DCID to monitor the offset for every 10 samples on captured in an interval
		// configured in the DCID DCI_SNTP_UPDATE_TIME
		m_owner_sntp_time_offset->Check_In( &max_offset_in_samples );

		// overriding the samples for every 10 count
		m_sntp_time_offset_index = 0U;
	}
#endif

	time_offset[0] = result / SEC_TO_MICRO_SEC_SCALE;
	time_offset[1] = result % SEC_TO_MICRO_SEC_SCALE;

	offset[0] = ntohl( time_offset[0] );
	offset[1] = ntohl( time_offset[1] * SNTP_PICO_TO_MICRO_SEC_SCALE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD SNTP_If::Set_Sntp_Server_List( uint8_t* time_server, DCI_Owner* dci_owner_sntp_server,
											 uint8_t server_index, DCI_ACCESS_ST_TD* access_struct )
{
	uint32_t sntp_server_temp = 0U;
	uint8_t sntp_server_conv[MAX_IP_OCTETS] = {0};
	char_t* test_string = nullptr;
	uint16_t test_string_len = 0U;
	uint8_t temp_string[MAX_SNTP_SERVER_NAME_LENGTH];
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	uint8_t result = 0U;

	if ( time_server != nullptr )
	{
		test_string_len = dci_owner_sntp_server->Get_Length();

		if ( access_struct == nullptr )
		{
			if ( dci_owner_sntp_server->Check_Out( time_server ) )
			{
				return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
			}
		}
		else
		{
			BF_Lib::Copy_Const_String( time_server, reinterpret_cast<uint8_t*>( access_struct->data_access.data ),
									   test_string_len );
		}

		///< Converting string to bytes to validate the IP address
		BF_Lib::Copy_Val_To_String( ( uint8_t* )temp_string, 0, test_string_len );
		BF_Lib::Copy_Const_String( temp_string, time_server, test_string_len );
		test_string = reinterpret_cast<char_t*>( temp_string );

#ifdef SNTP_DOMAIN_NAME_SUPPORT
		// Actual string length in the time server by parsing the NULL
		uint16_t actual_string_len = BF_Lib::Get_String_Length( time_server, test_string_len );
#endif
		///< Validate the string has a valid IPv4 Address octes seperated by '.'
		result = SNTP_Validate_And_Parse_IPv4_Address( test_string, sntp_server_conv );
		if ( result == MAX_IP_OCTETS )
		{
			BF_Lib::Align_String_U8_To_U32( &sntp_server_temp, sntp_server_conv,
											static_cast<uint32_t>( MAX_IP_OCTETS ) );
			///< Set the sntp server array with IP Address
#ifdef ESP32_SETUP
			sntp_server_temp = lwip_htonl( sntp_server_temp );
			sntp_setserver( server_index, ( const ip_addr_t* )&sntp_server_temp );
#else
			sntp_setserver_with_L_endian_ip( server_index, &sntp_server_temp );
#endif
		}
#ifdef SNTP_DOMAIN_NAME_SUPPORT
#ifdef ESP32_SETUP
		///< Sanity check on the string has atleast minimum length of 12 (i.e)pool.ntp.org
		else if ( ( result != INVALID_SERVER ) && ( actual_string_len >= MIN_SERVER_LEN_FOR_TEST ) )
#else
		///< Sanity check on the string has atleast minimum length of 13 (i.e).pool.ntp.org
		else if ( ( result != INVALID_SERVER ) && ( actual_string_len > MIN_SERVER_LEN_FOR_TEST ) )
#endif
		{
			test_string = reinterpret_cast<char_t*>( time_server );
			///< Set the sntp server array with Hostname
			sntp_setservername( server_index, test_string );
		}
#endif
		else
		{
			///< if string entered by the user is not having mandatory string length
			///< then return error code "incorrect data"
			return_status = DCI_CBack_Encode_Error( DCI_ERR_DATA_INCORRECT );
		}
	}
	return ( return_status );
}

#ifdef SNTP_DOMAIN_NAME_SUPPORT
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD SNTP_If::Sntp_Dns_Check_Whitelist_Sanitize( DCI_ACCESS_ST_TD* access_struct )
{

	uint16_t test_string_len = 0U;
	uint32_t return_val = 0U;
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	test_string_len = access_struct->data_access.length;

	///< Before set the value to RAM ptr of the DCID , sanitize and validate the characters
	return_val = sntp_dns_whitelist.Sanitize( reinterpret_cast<char_t*>( access_struct->data_access.data ),
											  test_string_len );

	///< if any of the character entered by the user is not valid then return error code "incorrect data"
	if ( return_val != test_string_len )
	{
		return_status = DCI_CBack_Encode_Error( DCI_ERR_DATA_INCORRECT );
	}
	return ( return_status );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
SNTP_If::~SNTP_If( void )
{
	delete m_sntp_server_1;
	delete m_sntp_server_2;
	delete m_sntp_server_3;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t SNTP_If::SNTP_Validate_And_Parse_IPv4_Address( char_t* ip_add, void* raw_data )
{
	uint16_t num = 0U;
	bool status = false;
	bool loop_break = false;
	bool token_status = true;
	char_t* octet_bk = nullptr;
	uint8_t received_octet = 0;
	uint8_t* raw_ip_data = reinterpret_cast<uint8_t*>( raw_data );
	uint16_t temp_buffer[MAX_IP_OCTETS] = {0};
	uint8_t valid_octet = 0;

	if ( ip_add != nullptr )
	{
		char_t* cur_str = reinterpret_cast<char_t*>( ip_add );

		// Read first ip octet from ip address string
		// e.g. Read "192" from "192.168.1.254"
		char_t* octet = Strtok_Search( cur_str, IP_DELIMITER );

		while ( loop_break == false )
		{
			if ( ( octet != nullptr ) ||								// > Next octet present
				 ( ( octet == nullptr ) && ( received_octet > 0 ) ) )	// > Last octet
			{
				if ( octet == nullptr )
				{
					// We are calculating address of next octet since previous strtok()
					// delimiter search failed due to last tocken entry.
					token_status = false;
					octet = octet_bk + strlen( octet_bk );
					octet++;
				}
				else
				{
					octet_bk = octet;
				}

				num = strtol( octet, nullptr, 10U );		// Covert into decimal number
				if ( Is_Decimal_String( octet ) == true )	// Octet should have only digits
				{
					received_octet++;

					if ( ( ( received_octet < MAX_IP_OCTETS ) && ( token_status == true ) ) ||	// Next tocken avaialble
						 ( ( received_octet == MAX_IP_OCTETS ) && ( token_status == false ) ) )	// Last tocken
					{
						temp_buffer[MAX_IP_OCTETS - received_octet] = num;

						if ( received_octet == MAX_IP_OCTETS )
						{
							// Success: Ip data parsed successfully
							loop_break = true;
							status = true;
						}
						else
						{
							octet = Strtok_Search( nullptr, IP_DELIMITER );
						}
					}
					else
					{
						// Error: Less than or More than 4 octet received
						loop_break = true;
						status = false;
					}
				}
				else
				{
					// Error: Incorrect ip octet value, atleast decimal number should be present
					loop_break = true;
				}
			}
			else
			{
				// Error: IP Delimiter not found
				loop_break = true;
			}
		}
	}
	if ( status == true )
	{
		for ( uint8_t loop_var = 0; loop_var < MAX_IP_OCTETS; loop_var++ )
		{
			valid_octet++;
			if ( temp_buffer[MAX_IP_OCTETS - valid_octet] > MAX_8BIT_NUMBER )
			{
				// Since the String has only decimal numbers with IP Delimiters
				// Error: Incorrect ip octet value. Valid range 0 to 255.
				// Declared the string received does not have a valid host name
				received_octet = INVALID_SERVER;
				break;
			}
			else
			{
				raw_ip_data[MAX_IP_OCTETS - valid_octet] = temp_buffer[MAX_IP_OCTETS - valid_octet];
			}
		}
	}
	else
	{
		// if the string has wrong IP address with more octets.
		if ( received_octet >= MAX_IP_OCTETS )
		{
			received_octet = INVALID_SERVER;
		}
	}
	return ( received_octet );
}
